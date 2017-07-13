/*
 * floppy driver.
 *
 * Inspired by floppy driver in Minix
 *
 */

#include "io.h"
#include "int.h"
#include "irq.h"
#include "floppy.h"
#include "mem.h"
#include "screen.h"
#include "kthread.h"
#include "string.h"
#include "timer.h"

typedef struct {
    char motorOn;
    int noMotorOffDelay;
    int motorOffTimerID;
    short curr_Cylinder;
    short num_Heads;
    short num_Cylinders;
    short num_SectorsPerTrack;
    short num_BytesPerSector;
    int currCylinder;
} floppyDisk;

// place to read/write data
int dmaPage;

int floppyDebug = 0;
static int remaingTime;
static int bochsEmulator =1;
static int floppyNeedsReset;
static floppyDisk FloppyDrive[2];

#ifdef AFTER_PROJ_P1
static struct Thread_Queue s_floppyWaitQueue = THREAD_QUEUE_INITIALIZER;
static struct Thread_Queue s_floppySleepQueue = THREAD_QUEUE_INITIALIZER;
static struct Mutex floppyDriverLock = MUTEX_INITIALIZER;
#else
static volatile int floppyDone=0;
#endif

static int printOutErrors = 1;
static int Floppy_In();
static void Floppy_Out(int);
static void Floppy_Reset();
static int Floppy_GetResult(char *);
static int Floppy_Recalibrate(int drive);
static int Floppy_Seek(int drive, int head, int cylinder);
static int Floppy_Sleep(int delay);

static void  readDriveConfig(int drive)
{
    FloppyDrive[drive].motorOn = 0;
    FloppyDrive[drive].num_Heads = 2;
    FloppyDrive[drive].num_Cylinders = 80;
    FloppyDrive[drive].num_SectorsPerTrack = 18;
    FloppyDrive[drive].num_BytesPerSector = 512;
}

static char result[FLOPPY_MAX_RESULT_BYTES];

//
// callback from timer
//
void floppyTimeOutCallback(int id)
{
    remaingTime = 0;

#ifdef AFTER_PROJ_P1
    if (FloppyDrive[0].motorOffTimerID > 0)
	Cancel_Timer(FloppyDrive[0].motorOffTimerID);
    FloppyDrive[0].motorOffTimerID = -1;
#endif

#ifdef AFTER_PROJ_P1
    Wake_Up(&s_floppyWaitQueue);
#endif
}

void Start_Motor(int drive)
{
    int command;

    Disable_Interrupts();

    // stop the motor stop timer
#ifdef AFTER_PROJ_P1
    if (FloppyDrive[drive].motorOffTimerID > 0)
	Cancel_Timer(FloppyDrive[drive].motorOffTimerID);
#endif
    FloppyDrive[drive].motorOffTimerID = -1;

    command = 0x10 << drive | FLOPPY_INTERRUPT_ENABLE;

    Out_Byte(FLOPPY_DIGITAL_OUTPUT_REGISTER, command);

    if (!FloppyDrive[drive].motorOn) {
	if (!bochsEmulator) Floppy_Sleep(18);
    }

    FloppyDrive[drive].motorOn = 1;

    Enable_Interrupts();
}

void reallyStopMotor(int timerID)
{
    int command;

    if (floppyDebug) Print("stopping floppy drive motor\n");

    command = 0x00;
    Out_Byte(FLOPPY_DIGITAL_OUTPUT_REGISTER, command);

    while ((In_Byte(FLOPPY_MAIN_STATUS_REGISTER) & FLOPPY_BUSY));

#ifdef AFTER_PROJ_P1
    Cancel_Timer(timerID);
#endif

    FloppyDrive[0].motorOn = 0;
    FloppyDrive[0].motorOffTimerID = -1;
}

void Stop_Motor(int drive)
{
    Disable_Interrupts();

    if (FloppyDrive[drive].noMotorOffDelay) {
        Print("fd: doing an immediate motor stop\n");
	reallyStopMotor(FloppyDrive[drive].motorOffTimerID);
    } else {
	// start a timer in about 10 seconds to stop the motor
#ifdef AFTER_PROJ_P1
	if (FloppyDrive[drive].motorOffTimerID > 0)
	    Cancel_Timer(FloppyDrive[drive].motorOffTimerID);
	FloppyDrive[drive].motorOffTimerID = Start_Timer(18 * 10, reallyStopMotor);
#endif
    }

    Enable_Interrupts();
}

int GetWriteProtected()
{
    int ret;

    Out_Byte(FLOPPY_CONTROL_REGISTER, FLOPPY_GETSTATUS);

    while (!In_Byte(FLOPPY_MAIN_STATUS_REGISTER) & FLOPPY_BUSY);

    ret = In_Byte(FLOPPY_CONTROL_REGISTER);

    return ret & 0x40;

}

char buffer[512];

static char senseBytes[FLOPPY_MAX_RESULT_BYTES];

static void Floppy_Interrupt_Handler( struct Interrupt_State* state )
{
    Begin_IRQ( state );

    if (floppyDebug) Print("fd: in interrupt handler\n");

#ifdef AFTER_PROJ_P1
    Wake_Up(&s_floppyWaitQueue);
#else
    floppyDone =1;
#endif

    End_IRQ( state );
}


//
// Wait for a floppy interrupt
//      Should be called with interrupts disabled
//
static int Floppy_Await_Interrupt(int doSense)
{
#ifdef AFTER_PROJ_P1
    int timerID;
#endif

    int senseLimit;
    int FloppySenseByteCount = 0;
    int FloppyResultByteCount = 0;

    KASSERT(!Interrupts_Enabled());
 
#ifdef AFTER_PROJ_P1
    // schedule a watch dog timer 
    timerID = Start_Timer(50, floppyTimeOutCallback);

    remaingTime = 50;

    // wait for interrupt 
    Wait(&s_floppyWaitQueue);
    Cancel_Timer(timerID);

    if (!remaingTime) {
	if (floppyDebug) Print("timer expired\n");
	floppyNeedsReset = 1;

	Enable_Interrupts();
	return -1;
    }
#else

    if (floppyDebug) Print("fd: about to spin for interrupt\n");
    Enable_Interrupts();
    while (!floppyDone);
    Disable_Interrupts();
    floppyDone = 0;
    if (floppyDebug) Print("fd: past spin for interrupt\n");
#endif

    // check for status of completed command
    FloppyResultByteCount = Floppy_GetResult(result);
    if ((floppyDebug > 3) && !doSense) {   
	int i;

	Print("count = %d\n", FloppyResultByteCount);
	for (i=0; i < FloppyResultByteCount; i++) {
	    Print("%x ", result[i] & 0xff);
	}
	Print("\n");
    }

    if (doSense) {
	printOutErrors = 0;
	for (senseLimit=4; senseLimit; senseLimit--) {
	    Floppy_Out(FLOPPY_SENSE);
	    senseBytes[0] = Floppy_In();
	    senseBytes[1] = Floppy_In();
	    FloppySenseByteCount = 2;
	    if ((senseBytes[0] & 0x83) != FLOPPY_DRIVE_0) {
		// Print("fd: floppy drive sense failed try #%d\n", 4-senseLimit);
	    } else {
		if (floppyDebug) Print("fd: floppy drive sense suceeded, at track = %d\n", senseBytes[1]);
		break;
	    }
	    In_Byte(0x80);		// delay
	}

	printOutErrors = 0;

	if (!senseLimit) {
	    if (FloppySenseByteCount != 2) {
		Print("fd: Error, sense returned %d bytes not 3\n", 
		    FloppySenseByteCount);
	    }

	    Print("fd: Error getting SENSE info in irq handler\n");
	    floppyNeedsReset = 1;
	} else {
	    floppyNeedsReset = 0;
	}
    }

    Enable_Interrupts();

    return FloppyResultByteCount;
}

void Init_Floppy()
{
    int status;
    int irqMask;

    if (floppyDebug) Print("in init floppy drive\n");

    // detect if running under bochs
    Out_Byte(0x80, 0xff);
    status = In_Byte(0xe9);
    if (status == 0xe9) {
	bochsEmulator = 1;
	Print("running under bochs\n");
    }

    dmaPage = (int) Alloc_Page_Atomic();
    KASSERT((int) dmaPage < 1024*1024);

    // setup interrupts
    Install_IRQ( FLOPPY_IRQ, &Floppy_Interrupt_Handler );

    // Enable the IRQ in the IRQ mask
    irqMask = Get_IRQ_Mask();
    irqMask &= ~(1 << FLOPPY_IRQ);
    Set_IRQ_Mask( irqMask );

    Floppy_Reset();

    readDriveConfig(0);

    status = In_Byte(FLOPPY_MAIN_STATUS_REGISTER);
    if (floppyDebug) Print("floppy drive status = %x\n", status);
}

/*
 * return the number of logical blocks for a particular drive.
 *
 */
int Floppy_getNumBlocks(int driveNum)
{
     return -1;
}

void Floppy_Out(int command)
{
    int status;
    int retry = FLOPPY_MAX_RETRY;

    while (retry--) {
	status = In_Byte(FLOPPY_MAIN_STATUS_REGISTER);
	status &= (FLOPPY_MASTER | FLOPPY_DIRECTION);
	if (status == FLOPPY_MASTER) {
	    Out_Byte(FLOPPY_CONTROL_REGISTER, command);
	    return;
	} else {
	    In_Byte(0x80);		// delay
	}
    }

    if (printOutErrors) Print("fd: retry error on floppy drive\n");
}

int Floppy_In()
{
    int i;
    char status;

    for (i = 0; i < FLOPPY_MAX_RETRY; i++) {
        status = In_Byte(FLOPPY_MAIN_STATUS_REGISTER);
        status = In_Byte(FLOPPY_MAIN_STATUS_REGISTER);
	if ((status & 0xd0) == 0xd0) {
	    return In_Byte(FLOPPY_CONTROL_REGISTER);
	}
	In_Byte(0x80);		// delay
    }

    return -1;
}

int Floppy_GetResult(char *buffer)
{
    int i;
    int ret;
    int status;

    memset(buffer, '\0', FLOPPY_MAX_RESULT_BYTES);

    for (i=0; i < FLOPPY_MAX_RESULT_BYTES; i++) { 
	ret = Floppy_In();

	buffer[i] = ret & 0xff;

	status = In_Byte(FLOPPY_MAIN_STATUS_REGISTER);
	if (!(status & FLOPPY_BUSY)) return i;
    }

    return i;
}

static int Floppy_Seek(int drive, int head, int cylinder)
{
    int count;

    if (floppyDebug) Print("fd%d: seek head = %d, cylinder = %d, curr = %d\n", 
        drive, head, cylinder, FloppyDrive[drive].currCylinder);

    // if (FloppyDrive[drive].currCylinder == cylinder) return 0;

    Start_Motor(drive);

    Disable_Interrupts();

    FloppyDrive[drive].currCylinder = -1;

    Floppy_Out(FLOPPY_SEEK);
    Floppy_Out(8);			// drive a:
    // Floppy_Out(head << 2 | drive);
    Floppy_Out(cylinder);

    // wait for interrupt
    count = Floppy_Await_Interrupt(1);

    if (!bochsEmulator) Floppy_Sleep(6);			// 32msec

    Stop_Motor(drive);

    if (count < 0) return -1;

    if ((senseBytes[FLOPPY_ST0] & FLOPPY_ST0_MASK) != FLOPPY_ST0_SEEK_END) {
        Print("fd0: error during seek\n");
	return -1;
    }

    if (senseBytes[FLOPPY_ST1] != cylinder) {
        Print("fd0: error seek ended at %d, not %d\n", result[FLOPPY_ST1], cylinder);
	return -1;
    }

    FloppyDrive[drive].currCylinder = cylinder;
    if (floppyDebug) Print("fd%d: seek exited ok.\n", drive);


    return 0;
}

void ReadyDMA(int mode)
{
    int lowByte, middleByte, highByte;

    lowByte = dmaPage & 0xff;
    middleByte = (dmaPage >> 8) & 0xff;
    highByte = (dmaPage >> 16) & 0xff;
    
    if (floppyDebug > 3) Print("fd: in readyDMA\n");

    Disable_Interrupts();
    Out_Byte(DMA_M2, mode);
    Out_Byte(DMA_M1, mode);
    Out_Byte(DMA_ADDR, lowByte);
    Out_Byte(DMA_ADDR, middleByte);
    Out_Byte(DMA_TOP, highByte);
    
    // two bytes of size
    Out_Byte(DMA_COUNT, 511 & 0xff);
    Out_Byte(DMA_COUNT, (511 >> 8) & 0xff);
    Enable_Interrupts();

    Out_Byte(DMA_INIT, 2);

    if (floppyDebug > 3) Print("fd: leaving readyDMA\n");
}


/*
 * Transfer a block to/from the desired buffer.
 *
 */
static int Floppy_Transfer(int direction, int driveNum, int blockNum, char *buffer)
{
    int i;
    int count;
    int head, cylinder, sector;

#ifdef AFTER_PROJ_P1
    Mutex_Lock(&floppyDriverLock);
#endif

    if (direction == FLOPPY_WRITE) {
        memcpy((char *) dmaPage, (char *) buffer, 512);
    }

    Start_Motor(driveNum);

    if (floppyDebug > 3) Print("fd0: in transfer (%s)\n", direction == FLOPPY_READ ? "read": "write");

    /* now compute the head, cylinder, and sector */
    sector = blockNum % FloppyDrive[driveNum].num_SectorsPerTrack + 1;
    cylinder = blockNum / (FloppyDrive[driveNum].num_Heads * FloppyDrive[driveNum].num_SectorsPerTrack);
    head = (blockNum / FloppyDrive[driveNum].num_SectorsPerTrack) % FloppyDrive[driveNum].num_Heads;

    if (floppyDebug) Print("fd%d: transfer at sector = %d, cylinder = %d, head = %d\n",
    	driveNum, sector, cylinder, head);

    for (i=0; i < 4; i++) {
	Start_Motor(0);

	// seek drive
	Floppy_Seek(driveNum, head, cylinder);

	// set data rate
	Out_Byte(FLOPPY_DIGITAL_INPUT_REGISTER, 0);		// 500K/sec

	// do dma setup 
	ReadyDMA(direction == FLOPPY_READ ? DMA_READ: DMA_WRITE);

	Disable_Interrupts();
	Floppy_Out(direction);
	Floppy_Out(head << 2 | driveNum);
	Floppy_Out(cylinder);
	Floppy_Out(head);
	Floppy_Out(sector);
	Floppy_Out(0x02);		// always 0x02 --> 512 bytes/sector
	Floppy_Out(18);		// always 18 -->  18 sectors/track
	Floppy_Out(0x1c);		// gap
	Floppy_Out(0xff);

	// wait for interrupt
	count = Floppy_Await_Interrupt(0);

	if ((count > 0) && (result[0] & 0xc0) == 0) break;

	if (floppyDebug) Print("fd%d: transfer retry at sector = %d, cylinder = %d, head = %d\n", driveNum, sector, cylinder, head);

	Floppy_Reset();
    }

    // XXX - Check the results cylinder,head,sector indicates transfer matches
    if (direction == FLOPPY_READ) {
        memcpy(buffer, (char *) dmaPage, 512);
    }

#ifdef AFTER_PROJ_P1
    Mutex_Unlock(&floppyDriverLock);
#endif


    Stop_Motor(0);

    if (i== 4) {
	Print("fd0: transfer failed after 4 retries\n");
	while(1);
        return -1;
    } else {
	if (floppyDebug) Print("fd0: transfer sucessfull\n");
	return 0;
    }
}

/*
 * Read a block at the logical block number indicated.
 */
int Floppy_Read(int driveNum, int blockNum, char *buffer)
{
    return Floppy_Transfer(FLOPPY_READ, driveNum, blockNum, buffer);
}


/*
 * Write a block at the logical block number indicated.
 */
int Floppy_Write(int driveNum, int blockNum, char *buffer)
{
    return Floppy_Transfer(FLOPPY_WRITE, driveNum, blockNum, buffer);
}

void sleepInterrupt(int id)
{
#ifdef AFTER_PROJ_P1
    Wake_Up(&s_floppySleepQueue);

    Cancel_Timer(id);
#endif
}

static int Floppy_Sleep(int delay)
{
#ifdef AFTER_PROJ_P1
    int timer;
#endif
    int reenableInt = 0;

    if (Interrupts_Enabled()) {
        reenableInt = 1;
        Disable_Interrupts();
    }

#ifdef AFTER_PROJ_P1
    timer = Start_Timer(delay, sleepInterrupt);

    Wait(&s_floppySleepQueue);
#endif

    if (reenableInt) Enable_Interrupts();

    return 0;
}

static void Floppy_Reset()
{

    if (floppyDebug) Print("fd: in reset\n");

    Disable_Interrupts();

    Out_Byte(FLOPPY_DIGITAL_OUTPUT_REGISTER, 0);

    if (!bochsEmulator) Floppy_Sleep(18);

    Out_Byte(FLOPPY_DIGITAL_OUTPUT_REGISTER, FLOPPY_INTERRUPT_ENABLE);

    // Out_Byte(FLOPPY_MAIN_STATUS_REGISTER, 0);

    // wait for interrupt
if (1) {
    int count;
    if (floppyDebug) Print("fd: about to wait interrupts\n");
    count = Floppy_Await_Interrupt(0);

    if (floppyDebug) Print("fd: past interrupt for reset\n");

    Disable_Interrupts();
}


    Floppy_Out(FLOPPY_SPECIFY);
    Floppy_Out(FLOPPY_SPEC_PARAM1);
    Floppy_Out(FLOPPY_SPEC_PARAM2);

    if (!bochsEmulator) Floppy_Sleep(18);

    Enable_Interrupts();

    Floppy_Recalibrate(0);
}

static int Floppy_Recalibrate(int drive)
{
    int count;

    if (floppyDebug) Print("fd%d: in recalibrate\n", drive);
    Start_Motor(drive);

    Disable_Interrupts();

    FloppyDrive[drive].currCylinder = -1;

    Floppy_Out(FLOPPY_RECALIBRATE);
    // Floppy_Out(drive);
    Floppy_Out(8);		// select drive A:

    // wait for interrupt
    count = Floppy_Await_Interrupt(1);

    FloppyDrive[drive].curr_Cylinder = -1;
    if ((senseBytes[0] & FLOPPY_ST0_MASK) != FLOPPY_ST0_SEEK_END) {
        // recalibrate failed
	Print("fd0: Recalibrate failed, count = %d, sense[0] = %x\n",
	    count, senseBytes[0]);
	return -1;
    }

    if (floppyDebug) Print("fd0: exiting recalibrate for drive = %d\n", drive);

    Stop_Motor(drive);

    return 0;
}


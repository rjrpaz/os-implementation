/*
 * ATA (aka IDE) driver.
 *
 */

#include "io.h"
#include "int.h"
#include "ide.h"
#include "screen.h"

typedef struct {
    short num_Cylinders;
    short num_Heads;
    short num_SectorsPerTrack;
    short num_BytesPerSector;
} ideDisk;

int ideDebug = 0;
int numDrives = 1;
static ideDisk drives[2];


static int readDriveConfig(int drive)
{
    int i;
    int status;
    short info[256];
    long long int bytes;

    if (ideDebug > 1) Print("ide: about to read drive config for drive #%d\n", 
        drive);

    Out_Byte(IDE_DRIVE_HEAD_REGISTER, (drive == 0) ? IDE_DRIVE_0 : IDE_DRIVE_1);
    Out_Byte(IDE_COMMAND_REGISTER, IDE_COMMAND_IDENTIFY_DRIVE);
    while (In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);

    status = In_Byte(IDE_STATUS_REGISTER);
    // simulate failure
    // status = 0x50;
    if ((status & IDE_STATUS_DRIVE_DATA_REQUEST)) {
       Print("ide: probe found ATA drive\n");
         // drive responded to ATA probe
	for (i=0; i < 256; i++) {
	    info[i] = In_Word(IDE_DATA_REGISTER);
	}

	drives[drive].num_Cylinders = info[IDE_INDENTIFY_NUM_CYLINDERS];
	drives[drive].num_Heads = info[IDE_INDENTIFY_NUM_HEADS];
	drives[drive].num_SectorsPerTrack = info[IDE_INDENTIFY_NUM_SECTORS_TRACK];
	drives[drive].num_BytesPerSector = info[IDE_INDENTIFY_NUM_BYTES_SECTOR];
    } else {
       // try for ATAPI
       Out_Byte(IDE_FEATURE_REG, 0);		// disable dma & overlap

       Out_Byte(IDE_DRIVE_HEAD_REGISTER, (drive == 0) ? IDE_DRIVE_0 : IDE_DRIVE_1);
       Out_Byte(IDE_COMMAND_REGISTER, IDE_COMMAND_ATAPI_IDENT_DRIVE);
       while (In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);
       status = In_Byte(IDE_STATUS_REGISTER);
       Print("ide: found atapi drive\n");
       return -1;
    }

    Print ("Found IDE: Drive %d\n", drive);
    Print ("    %d cylinders, %d heads, %d sectors/tack, %d bytes/sector\n", 
        drives[drive].num_Cylinders, drives[drive].num_Heads,
        drives[drive].num_SectorsPerTrack, drives[drive].num_BytesPerSector);
    bytes = ((long long int )IDE_getNumBlocks(drive)) * 512;
    Print ("    Disk has %d blocks (%l bytes)\n", IDE_getNumBlocks(drive), bytes);

    return 0;
}


void Init_IDE()
{
    int ret;
    int errorCode;

    while (In_Byte(IDE_STATUS_REGISTER) != 0x50);

    if (ideDebug) Print("About to run drive Diagnosis\n");

    Out_Byte(IDE_COMMAND_REGISTER, IDE_COMMAND_DIAGNOSTIC);
    while (In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);
    errorCode = In_Byte(IDE_ERROR_REGISTER);
    if (ideDebug > 1) Print("ide: ide error register = %x\n", errorCode);

    ret = readDriveConfig(0);
    if (ret < 0) {
        numDrives = 0;
	return;
    } else if (errorCode & 0x80) {
	if (ideDebug > 1) Print("ide: found one drive\n");
        numDrives = 1;
    } else {
	if (ideDebug > 1) Print("ide: found second drive\n");
        numDrives = 2;
	readDriveConfig(1);
    }
}

/*
 * return the number of logical blocks for a particular drive.
 *
 */
int IDE_getNumBlocks(int driveNum)
{
    if (driveNum < 0 || driveNum > (numDrives-1)) {
        return IDE_ERROR_BAD_DRIVE;
    }

    return (drives[driveNum].num_Heads * 
            drives[driveNum].num_SectorsPerTrack *
	    drives[driveNum].num_Cylinders);
}

/*
 * Read a block at the logical block number indicated.
 */
int IDE_Read(int driveNum, int blockNum, char *buffer)
{
    int i;
    int head;
    int sector;
    int cylinder;
    short *bufferW;
    int reEnable = 0;

    if (driveNum < 0 || driveNum > (numDrives-1)) {
	if (ideDebug) Print("ide: invalid drive %d\n", driveNum);
        return IDE_ERROR_BAD_DRIVE;
    }

    if (blockNum < 0 || blockNum >= IDE_getNumBlocks(driveNum)) {
	if (ideDebug) Print("ide: invalid block %d\n", blockNum);
        return IDE_ERROR_INVALID_BLOCK;
    }

    if (Interrupts_Enabled()) {
	Disable_Interrupts();
	reEnable = 1;
    }

    /* now compute the head, cylinder, and sector */
    sector = blockNum % drives[driveNum].num_SectorsPerTrack + 1;
    cylinder = blockNum / (drives[driveNum].num_Heads * 
     	drives[driveNum].num_SectorsPerTrack);
    head = (blockNum / drives[driveNum].num_SectorsPerTrack) % 
        drives[driveNum].num_Heads;

    if (ideDebug) {
	Print ("request to read block %d\n", blockNum);
	Print ("    head %d\n", head);
	Print ("    cylinder %d\n", cylinder);
	Print ("    sector %d\n", sector);
    }

    Out_Byte(IDE_SECTOR_COUNT_REGISTER, 1);
    Out_Byte(IDE_SECTOR_NUMBER_REGISTER, sector);
    Out_Byte(IDE_CYLINDER_LOW_REGISTER, LOW_BYTE(cylinder));
    Out_Byte(IDE_CYLINDER_HIGH_REGISTER, HIGH_BYTE(cylinder));
    if (driveNum == 0) {
	Out_Byte(IDE_DRIVE_HEAD_REGISTER, IDE_DRIVE_0 | head);
    } else if (driveNum == 1) {
	Out_Byte(IDE_DRIVE_HEAD_REGISTER, IDE_DRIVE_1 | head);
    }

    Out_Byte(IDE_COMMAND_REGISTER, IDE_COMMAND_READ_SECTORS);

    if (ideDebug) Print("About to wait for Read \n");

    /* wait for the drive */
    while (In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);

    if (In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_ERROR) {
	Print("ERROR: Got Read %d\n", In_Byte(IDE_STATUS_REGISTER));
	return IDE_ERROR_DRIVE_ERROR;
    }

    if (ideDebug) Print("got buffer \n");

    bufferW = (short *) buffer;
    for (i=0; i < 256; i++) {
        bufferW[i] = In_Word(IDE_DATA_REGISTER);
    }

    if (reEnable) Enable_Interrupts();

    return IDE_ERROR_NO_ERROR;
}


/*
 * Write a block at the logical block number indicated.
 */
int IDE_Write(int driveNum, int blockNum, char *buffer)
{
    int i;
    int head;
    int sector;
    int cylinder;
    short *bufferW;
    int reEnable = 0;

    if (driveNum < 0 || driveNum > (numDrives-1)) {
        return IDE_ERROR_BAD_DRIVE;
    }

    if (blockNum < 0 || blockNum >= IDE_getNumBlocks(driveNum)) {
        return IDE_ERROR_INVALID_BLOCK;
    }

    if (Interrupts_Enabled()) {
	Disable_Interrupts();
	reEnable = 1;
    }

    /* now compute the head, cylinder, and sector */
    sector = blockNum % drives[driveNum].num_SectorsPerTrack + 1;
    cylinder = blockNum / (drives[driveNum].num_Heads * 
     	drives[driveNum].num_SectorsPerTrack);
    head = (blockNum / drives[driveNum].num_SectorsPerTrack) % 
        drives[driveNum].num_Heads;

    if (ideDebug) {
	Print ("request to write block %d\n", blockNum);
	Print ("    head %d\n", head);
	Print ("    cylinder %d\n", cylinder);
	Print ("    sector %d\n", sector);
    }

    Out_Byte(IDE_SECTOR_COUNT_REGISTER, 1);
    Out_Byte(IDE_SECTOR_NUMBER_REGISTER, sector);
    Out_Byte(IDE_CYLINDER_LOW_REGISTER, LOW_BYTE(cylinder));
    Out_Byte(IDE_CYLINDER_HIGH_REGISTER, HIGH_BYTE(cylinder));
    if (driveNum == 0) {
	Out_Byte(IDE_DRIVE_HEAD_REGISTER, IDE_DRIVE_0 | head);
    } else if (driveNum == 1) {
	Out_Byte(IDE_DRIVE_HEAD_REGISTER, IDE_DRIVE_1 | head);
    }

    Out_Byte(IDE_COMMAND_REGISTER, IDE_COMMAND_WRITE_SECTORS);


    /* wait for the drive */
    while (In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);

    bufferW = (short *) buffer;
    for (i=0; i < 256; i++) {
        Out_Word(IDE_DATA_REGISTER, bufferW[i]);
    }

    if (ideDebug) Print("About to wait for Write \n");

    /* wait for the drive */
    while (In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);

    if (In_Byte(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_ERROR) {
	Print("ERROR: Got Read %d\n", In_Byte(IDE_STATUS_REGISTER));
	return IDE_ERROR_DRIVE_ERROR;
    }

    if (reEnable) Enable_Interrupts();

    return IDE_ERROR_NO_ERROR;
}

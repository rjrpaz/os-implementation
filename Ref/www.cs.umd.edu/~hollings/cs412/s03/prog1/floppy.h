
#define FLOPPY_IRQ			0x6

/* DMA related PORTS */
#define DMA_M1				0x00B
#define DMA_M2				0x00C
#define DMA_ADDR			0x004
#define DMA_TOP				0x081
#define DMA_COUNT			0x005
#define DMA_INIT			0x00A

/* DMA constants */
#define DMA_READ			0x46
#define DMA_WRITE			0x4A

/* Registers */
#define FLOPPY_DIGITAL_OUTPUT_REGISTER	0x3f2
#define FLOPPY_MAIN_STATUS_REGISTER	0x3f4
#define FLOPPY_CONTROL_REGISTER		0x3F5
#define FLOPPY_DIGITAL_INPUT_REGISTER	0x3f7

/* Drives */
#define FLOPPY_DRIVE_0			0x0
#define FLOPPY_DRIVE_1			0x1
#define FLOPPY_DRIVE_3			0x2
#define FLOPPY_DRIVE_4			0x3

/* Status Bits */
#define FLOPPY_BUSY_MASK		0x0F
#define FLOPPY_BUSY			0x10
#define FLOPPY_DMA_MODE			0x20
#define FLOPPY_DIRECTION		0x40
#define FLOPPY_MASTER			0x80
#define FLOPPY_RESET_OK			0xC0

/* control bits FLOPPY_DIGITAL_OUTPUT_REGISTER */
#define FLOPPY_INTERRUPT_ENABLE		0x0c
#define FLOPPY_MOTOR_BITS		0xf0

/* Commands FLOPPY_MAIN_STATUS_REGISTER */
#define FLOPPY_SPECIFY			0x03
#define FLOPPY_GETSTATUS		0x04
#define FLOPPY_RECALIBRATE		0x07
#define FLOPPY_SENSE			0x08
#define FLOPPY_SEEK 			0x0F
#define FLOPPY_WRITE			0xC5
#define FLOPPY_READ			0xE6

/* Constants to config drive */
#define FLOPPY_SPEC_PARAM1		0xDF
#define FLOPPY_SPEC_PARAM2		0x02

/* bytes of return codes */
#define FLOPPY_ST0			0
#define FLOPPY_ST1			1
#define FLOPPY_ST2			2
#define FLOPPY_ST3			0
#define FLOPPY_ST_CYL			0x03
#define FLOPPY_ST_HEAD			0x04
#define FLOPPY_ST_SEC			0x05

/* Bits in ST0 result */
#define FLOPPY_ST0_MASK			0xF8
#define FLOPPY_ST0_SEEK_END		0x20

/* Bits in ST1 result */
#define FLOPPY_ST1_BAD_SECTOR		0x05
#define FLOPPY_ST1_WRITE_PROTECT	0x02

/* Bits in ST2 result */
#define FLOPPY_ST2_BAD_CYL		0x1F

/* paramters */
#define FLOPPY_MAX_RETRY		20
#define FLOPPY_MAX_RESULT_BYTES		7
#define FLOPPY_STEPS_PER_CLYINDER	0

void Init_Floppy();
int Floppy_getNumDrives();
int Floppy_getNumBlocks(int driveNum);
int Floppy_Read(int driveNum, int blockNum, char *buffer);
int Floppy_Write(int driveNum, int blockNum, char *buffer);

#define LOW_BYTE(x)	(x & 0xff)
#define HIGH_BYTE(x)	((x >> 8) & 0xff)

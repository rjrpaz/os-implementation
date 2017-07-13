
/* Registers */
#define IDE_DATA_REGISTER		0x1f0
#define IDE_ERROR_REGISTER		0x1f1
#define IDE_SECTOR_COUNT_REGISTER	0x1f2
#define IDE_SECTOR_NUMBER_REGISTER	0x1f3
#define IDE_CYLINDER_LOW_REGISTER	0x1f4
#define IDE_CYLINDER_HIGH_REGISTER	0x1f5
#define IDE_DRIVE_HEAD_REGISTER		0x1f6
#define IDE_STATUS_REGISTER		0x1f7
#define IDE_COMMAND_REGISTER		0x1f7

/* Drives */
#define IDE_DRIVE_0			0xa0
#define IDE_DRIVE_1			0xb0

/* Commands */
#define IDE_COMMAND_IDENTIFY_DRIVE	0xec
#define IDE_COMMAND_SEEK		0x70
#define IDE_COMMAND_READ_SECTORS	0x21
#define IDE_COMMAND_READ_BUFFER		0xE4
#define IDE_COMMAND_WRITE_SECTORS	0x30
#define IDE_COMMAND_WRITE_BUFFER	0xE8
#define IDE_COMMAND_DIAGNOSTIC		0x90

/* Results words from Identify Drive Request */
#define	IDE_INDENTIFY_NUM_CYLINDERS	0x01
#define	IDE_INDENTIFY_NUM_HEADS		0x03
#define	IDE_INDENTIFY_NUM_BYTES_TRACK	0x04
#define	IDE_INDENTIFY_NUM_BYTES_SECTOR	0x05
#define	IDE_INDENTIFY_NUM_SECTORS_TRACK	0x06

/* bits of Status Register */
#define IDE_STATUS_DRIVE_BUSY		0x80
#define IDE_STATUS_DRIVE_READY		0x40
#define IDE_STATUS_DRIVE_WRITE_FAULT	0x20
#define IDE_STATUS_DRIVE_SEEK_COMPLETE	0x10
#define IDE_STATUS_DRIVE_DATA_REQUEST	0x08
#define IDE_STATUS_DRIVE_CORRECTED_DATA	0x04
#define IDE_STATUS_DRIVE_INDEX		0x02
#define IDE_STATUS_DRIVE_ERROR		0x01

/* Return codes from various IDE_* functions */
#define	IDE_ERROR_NO_ERROR	0
#define	IDE_ERROR_BAD_DRIVE	-1
#define	IDE_ERROR_INVALID_BLOCK	-2
#define	IDE_ERROR_DRIVE_ERROR	-3

void Init_IDE();
int IDE_getNumDrives();
int IDE_getNumBlocks(int driveNum);
int IDE_Read(int driveNum, int blockNum, char *buffer);
int IDE_Write(int driveNum, int blockNum, char *buffer);

#define LOW_BYTE(x)	(x & 0xff)
#define HIGH_BYTE(x)	((x >> 8) & 0xff)

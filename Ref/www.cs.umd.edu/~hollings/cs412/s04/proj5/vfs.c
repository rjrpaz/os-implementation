
#include "kthread.h"
#include "user.h"
#include "string.h"
#include "screen.h"
#include "vfs.h"
#include "pfat.h"
#include "gosfs.h"
#include "synch.h"
#include "fileio.h"
#include "malloc.h"

#define mountLimit 10

struct Mutex mountLock = MUTEX_INITIALIZER;

int mountCount;
mountPoint mounts[mountLimit];

int Mount(char *name, int drive, char *fstype)
{
    Mutex_Lock( &mountLock );

    if (mountCount >= mountLimit) {
	Print("mount table full\n");
	Mutex_Unlock( &mountLock );
    	return -1;
    }

    if (!strcmp(fstype, "pfat")) {
         // verify we can mount it
	 mounts[mountCount].fsType = FS_TYPE_PFAT;
    } else if (!strcmp(fstype, "gosfs")) {
	 mounts[mountCount].fsType = FS_TYPE_GOSFS;
    } else {
	Print("invalid fstype\n");
	Mutex_Unlock( &mountLock );
        return -1;
    }

    mounts[mountCount].mountPoint = strdup(name);
    mounts[mountCount].drive = drive;
    mountCount++;

    Print ("Mounted file system on drive %d of type %s onto %s\n", 
        drive, fstype, mounts[mountCount-1].mountPoint);

    Mutex_Unlock( &mountLock );

    return 0;
}

mountPoint *findMountPoint(char *name)
{
    int i;
    int match;
    mountPoint *ret;
    int matchLength;

    Mutex_Lock( &mountLock );

    match = 0;
    ret = NULL;
    matchLength = 0;
    for (i=0; i < mountCount; i++) {
        if (!strncmp(name, mounts[i].mountPoint, strlen(mounts[i].mountPoint))) {
	    if (strlen(mounts[i].mountPoint) > matchLength) {
	        match = i;
		matchLength = strlen(mounts[i].mountPoint);
		ret = &mounts[match];
	    }
	}
    }

    Mutex_Unlock( &mountLock );

    return ret;
}


int Open(char *name, int permissions)
{
    int i;
    int ret;
    mountPoint *mp;
    struct User_Context *u;

    u = g_currentThread->userContext;
    if (!u) {
        return -1;
    }

    // find mount point that applies
    mp = findMountPoint(name); 
    if (!mp) {
        Print("unable to find mount point for %s\n", name);
	return -1;
    }

    // find an open file table slot
    for (i=0; i < MAX_OPEN_FILES; i++) {
        if (u->files[i].fsType == FS_TYPE_NONE) {
	    break;
	}
    }
    if (i== MAX_OPEN_FILES) {
        Print("too many open files\n");
	return -1;
    }

    u->files[i].filePos = 0;
    u->files[i].mode = permissions;

    switch (mp->fsType) {
        case FS_TYPE_PFAT: {
	    char *data;
	    ret = ReadFileAndAllocate(name, &data, &u->files[i].endPos);
	    u->files[i].fsId = (int) data;
	    if (!ret) {
		u->files[i].fsType = FS_TYPE_PFAT;
		Print("open about to return %d\n", i);
		return i;
	    } else {
		Print("open about to return %d\n", -1);
		return -1;
	    }
	}

	case FS_TYPE_GOSFS: {
	    GOSFSptr *ptr;
	    ptr = GOSFS_Open(mp, &name[strlen(mp->mountPoint)], permissions);
	    if (ptr) {
		u->files[i].fsId = (int) ptr;
		u->files[i].fsType = FS_TYPE_GOSFS;
		return i;
	    }
	    return -1;
	}

	default: 
	    return -1;
    }

    return 0;
}

openFile *findOpenFilePtr(int fd)
{
    struct User_Context *u;

    u = g_currentThread->userContext;

    if (!u) return NULL;

    if (fd < 0 || fd > MAX_OPEN_FILES) {
	// attempt to close an invalid file
        return NULL;
    }

    if (u->files[fd].fsType == FS_TYPE_NONE) {
        return NULL;
    }

    return &(u->files[fd]);
}

int Close(int fd)
{
    openFile *of;

    of = findOpenFilePtr(fd);
    if (!of) return -1;

    switch (of->fsType) {
        case FS_TYPE_PFAT:
	    of->fsType = FS_TYPE_NONE;
	    Free_Atomic((void *) of->fsId);
	    return 0;
	
	case FS_TYPE_GOSFS: 
	    of->fsType = FS_TYPE_NONE;
	    return GOSFS_Close((GOSFSptr *) of->fsId);
	    break;

	default:
	    return -1;
    }
}

int Delete(char *file)
{
    mountPoint *mp;

    mp = findMountPoint(file); 
    if (!mp) return -1;

    switch (mp->fsType) {
        case FS_TYPE_PFAT:
	    return -1;
	
	case FS_TYPE_GOSFS: 
	    return GOSFS_Delete(mp, &file[strlen(mp->mountPoint)]);
	    break;

	default:
	    return -1;
    }
    return 0;
}

int Read(int fd, char *buffer, int length)
{
    openFile *of;

    of = findOpenFilePtr(fd);

    if (!of) return -1;

    switch (of->fsType) {
        case FS_TYPE_PFAT:
	    if (of->filePos >= of->endPos) {
		return -1;
	    }

	    if (of->filePos + length >= of->endPos) {
		length = of->endPos - of->filePos;
	    }
	    memcpy(buffer, (void *) (of->fsId + of->filePos), length);
	    of->filePos += length;
	    return length;
	
	case FS_TYPE_GOSFS: 
	    return GOSFS_Read((GOSFSptr *) of->fsId, buffer, length);
	    break;

	default:
	    return -1;
    }
}

int Write(int fd, char *buffer, int length)
{
    openFile *of;

    of = findOpenFilePtr(fd);
    if (!of) return -1;

    if (!(of->mode & O_WRITE)) {
	 // attempt to write to a file open for read
	 Print("attempt to write readonly file\n");
         return -1;
    }

    switch (of->fsType) {
        case FS_TYPE_PFAT: 
	    // read only file system;
	    return -1;
	    break;

	case FS_TYPE_GOSFS: 
	    return GOSFS_Write((GOSFSptr *) of->fsId, buffer, length);

	default:
	    return -1;
    }
}

int Stat(int fd, fileStat *stat)
{
    openFile *of;

    of = findOpenFilePtr(fd);
    if (!of) return -1;

    switch (of->fsType) {
        case FS_TYPE_PFAT: 
	    // read only file system;
	    stat->size = of->endPos;
	    stat->isDirectory = 0;
	    return 0;
	    break;

	case FS_TYPE_GOSFS: 
	    return GOSFS_Stat((GOSFSptr *) of->fsId, stat);
	    break;

	default:
	    return -1;
    }
    return -1;
}

int Seek(int fd, int offset)
{
    openFile *of;

    of = findOpenFilePtr(fd);
    if (!of) return -1;

    switch (of->fsType) {
        case FS_TYPE_PFAT: 
	    return -1;
	    break;

        case FS_TYPE_GOSFS: 
	    return GOSFS_Seek((GOSFSptr *) of->fsId, offset);
	    break;

	default:
	    return -1;
    }
    return -1;
}

int CreateDirectory(char *buffer)
{
    mountPoint *mp;

    mp = findMountPoint(buffer); 
    if (!mp) return -1;

    switch (mp->fsType) {
        case FS_TYPE_PFAT: 
	    return -1;
	    break;

        case FS_TYPE_GOSFS: 
	    return GOSFS_CreateDirectory(mp, &buffer[strlen(mp->mountPoint)+1]);

	default:
	    return -1;
    }
    return -1;
}

// You will implement this for project 5

int Format(int drive)
{
  /* may only format drive 1 */
  if (drive != 1) 
    return -1;
  else
    return GOSFS_Format(drive);
}

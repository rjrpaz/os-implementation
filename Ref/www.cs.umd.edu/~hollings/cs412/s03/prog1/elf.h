
typedef struct {
	unsigned  char	ident[16];
	unsigned  short	type;
	unsigned  short	machine;
	unsigned  int	version;
	unsigned  int	entry;
	unsigned  int	phoff;
	unsigned  int	sphoff;
	unsigned  int	flags;
	unsigned  short	ehsize;
	unsigned  short	phentsize;
	unsigned  short	phnum;
	unsigned  short	shentsize;
	unsigned  short	shnum;
	unsigned  short	shstrndx;
} elfHeader;

typedef struct {
   	unsigned  int   type;
   	unsigned  int   offset;
   	unsigned  int   vaddr;
   	unsigned  int   paddr;
   	unsigned  int   fileSize;
   	unsigned  int   memSize;
   	unsigned  int   flags;
   	unsigned  int   alignment;
} programHeader;

struct User_Program *loadElfProgram(char *programName);


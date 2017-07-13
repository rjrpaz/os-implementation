/*************************************************************************
**************************************************************************

This file contains memory allocation routines that do NOT use the BIOS
interrupts to obtain memory (thus, interrupts are not enabled by using
these calls). However, the system has a max allocated space determined by
32K*NUM_BLOCKS (which can be altered by changing the #define statement below
for NUM_BLOCKS).


			INITIAL STEPS FOR MODULE USE:

	1)  Init_malloc() must be called to initialize the free memory.
	  This call DOES use BIOS interrupts, and should be called during
	  initialization phase when interrupts are enabled.

	2)  Free_malloc() should be called before exiting the system. This
	  also uses BIOS interrupts and should be called when interrupts are
	  enabled.


			    PUBLIC ROUTINES:

	1. void *Safe_malloc(int size):  returns a pointer to free space whose
					size is >= 'size' (in bytes).

	2. void Safe_free(void *address): returns the segment pointed to by
					'address' to the list of free space.

	3. int Init_malloc():  uses 'malloc()' to create the free space.

	4. int Free_malloc():  returns the free space to the system using
			      'free()'.


	The free space is maintained using the boundary tag scheme where
a hole or a segment has tags placed just before and just after it that
indicate its size and its type ('H' for hole, 'S' for segment).  A doubly
linked list of hole pointers (stored in the hole space since it is unused)
allows us to search for a hole of the correct size using rotating first fit
method.  Once an appropriate hole has been found, the segment space is
"removed" from the hole, leaving a hole of (hole size - segment size) behind.


	Hole compaction is performed after a 'Safe_free', where adjacent holes
are combined and the linked list is altered if necessary.  The (static)
variable 'current' points to the starting hole for the next search.

**************************************************************************
**************************************************************************/


#include <dos.h>
#include <alloc.h>
#include <stdarg.h>
#include <string.h>
#include "cmsc412.h"
#include "screen.h"

#define FREE_SIZE 16000
#define SMALLEST_BLOCK 16
#define BYTE_TO_WORD(X)  ((X+1) >> 1)

#define INT_DISABLE 0
#define INT_ENABLE 0x200

#define NUM_BLOCKS 1

/* functions defined in this file that are used before their defn's */
static void insert (unsigned *newloc, unsigned *prevloc);
static void delete (unsigned *oldhole);
static void extract (unsigned *holeptr, long int segment_size, unsigned **rtnptr);
static unsigned *nextnode(unsigned *);


/* standard C lib functions */
int vsprintf(char *str, ...);
int abs(int);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++  GLOBAL POINTERS TO NEXT HOLE IN THE  HOLE LIST  +++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


unsigned *memory[NUM_BLOCKS],/* pointers to all free space,used for free() */
		*current;    /* used for rotating first-fit hole selection */

unsigned *testval;
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++++++++     STRUCTURES CONTAINED WITHIN EACH HOLE/SEGMENT        ++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


typedef struct             /*BOUNDARY TAG:  */
{
  char padding,                   /* makes the tag land on a word boundary */
	 type;                    /* type: 'H' (hole) OR 'S' (segment) */
  long int size;                        /* size: size of hole in words */
} tag;

#define TAG_SIZE BYTE_TO_WORD(sizeof(tag))


typedef struct holenode    /*  DOUBLY-LINKED LIST OF HOLES */
{
  struct holenode *prev,        /* prev: points to previous hole */
		  *succ;        /* succ:   "    "  next hole */
}holelist;


/***************************************************************************
****************************************************************************

		      HOLE/SEGMENT ACCESS ROUTINES:

	1. holepointer():  returns a pointer to a holenode.
	2. bottom_tag():      "    "    "     " the lower boundary tag.
	3. top_tag():         "    "    "     "  "  upper    "      " .
	4. above_neighbor():  "    "    "     "  "  upper neighbor.
	5. below_neighbor():  "    "    "     "  "  lower    "    .

****************************************************************************
***************************************************************************/



/***************************************************************************
PROCEDURE HOLEPOINTER TAKES THE HOLE POINTER AND RETURNS THE POINTER INTO
THE HOLE LIST ASSOCIATED WITH THE HOLE.
***************************************************************************/

static holelist *holepointer(unsigned *segmentstart)
{
  holelist *temp;

  temp = (holelist *)(segmentstart+BYTE_TO_WORD(sizeof(holelist)));
  return (temp);
}



/**************************************************************************
PROCEDURE NEAR_TAG RETURNS THE BOUNDARY TAG THAT LIES JUST BELOW THE
STRUCTURE POINTER.
**************************************************************************/

tag *bottom_tag(unsigned *segmentstart)
{
  unsigned *temp;

  /* THE TAG LIES JUST BELOW THE FREE SPACE */
  temp = segmentstart - TAG_SIZE;
  return((tag *)temp);
}



/**************************************************************************
PROCEDURE FAR_TAG RETURNS THE BOUNDARY TAG AT THE TOP OF THE STRUCTURE
*************************************************************************/

static tag *top_tag(unsigned *segmentstart)
{
  unsigned *temp;

  /* THE UPPER TAG LIES JUST BELOW THE FREE SPACE (POINT TO THE BEGINNING
     OF IT...) */
  temp = segmentstart + bottom_tag(segmentstart)->size;
  return((tag *)temp);
}


/*************************************************************************
PROCEDURE BELOW_NEIGHBOR RETURNS THE STRUCTURE THAT LIES JUST BELOW THE
GIVEN ONE (RETURNS null IF AT THE BOTTOM OF FREE SPACE)
*************************************************************************/

static unsigned *below_neighbor(unsigned *segmentstart)
{
  unsigned *temp, *neighbor_top_tag;

  /*THE NEIGHBOR'S TAG LIES JUST BELOW YOUR TAG */
  neighbor_top_tag = segmentstart - 2*TAG_SIZE;

/* INIT_MALLOC() PLACES A BOTTOM TAG (TYPE = 'B') AT THE BOTTOM OF THE BLOCK */
  if ( ((tag *)bottom_tag(segmentstart)-1)->type == 'B')
    return (NULL);



/*   START AT CURRENT SEG  BACK OVER BOTH TAGS    AND GO TO THE BOTTOM
	      |              |                       |
	      V              V                       V   */
  temp = segmentstart - 2*TAG_SIZE - ((tag *)neighbor_top_tag)->size;

  return (temp);
}


/*************************************************************************
PROCEDURE ABOVE_NEIGHBOR RETURNS THE POINTER TO THE STRUCTURE JUST ABOVE
THE GIVEN ONE (RETURNS null IF NONE ABOVE)
*************************************************************************/


static unsigned *above_neighbor(unsigned *segmentstart)
{
  unsigned *temp;


  /* INIT_MALLOC() PLACES A TOP TAG (TYPE = 'T') AT THE TOP OF THE BLOCK */
  if ( (top_tag(segmentstart)+1)->type == 'T')
    return (NULL);

  /* START AT CURRENT SEG   GO TO 1 PAST ITS TOP    SKIP OVER BOTH TAGS
	      |                      |                      |
	      V                      V                      V  */
  temp = segmentstart + bottom_tag(segmentstart)->size + 2*TAG_SIZE;
  return(temp);
}



/*************************************************************************
**************************************************************************
**************************************************************************

			  MAIN ROUTINES:

	1. Init_malloc():            initializes the free space.
	2. Safe_malloc(long S):  returns a pointer to a block(>=S) of bytes.
	3. Safe_free(void *ptr): adds the segment to the hole list.
	4. Free_malloc():            returns all Safe_malloc space to the system.

		      SUPPORTING ROUTINES:

	1. extract():   extracts a segment of a given size from the
			the given hole.
	2. coalesce():  combines adjacent holes into one hole.

**************************************************************************
*************************************************************************/


/*************************************************************************
PROCEDURE INIT_MALLOC USES malloc() TO GET THE FREE_SPACE, THEN
INITIALIZES THE POINTERS USED FOR THE ROTATING FIRST-FIT ALGORITHM.
*************************************************************************/

int Init_malloc()
{
  int i;
  unsigned *avail[NUM_BLOCKS];


  for (i = 0; i < NUM_BLOCKS; i++)
  {
    memory[i] =(unsigned *)malloc(2*FREE_SIZE);

    /* ERROR - NOT ENOUGH MEMORY */
    if (memory[i] == NULL)
	{
      current = NULL;
      return(0);
    }
    avail[i] = memory[i] + 2*TAG_SIZE;  /* LEAVE ROOM FOR THE BOUNDARY TAGS */

	/* INITIALIZE THE FREE SPACE AS ONE BIG HOLE */
    bottom_tag(avail[i])->type = 'H';
    bottom_tag(avail[i])->size = FREE_SIZE - 4*TAG_SIZE;
    top_tag(avail[i])->type = 'H';
    top_tag(avail[i])->size = FREE_SIZE - 4*TAG_SIZE;

    /* MARK THE TOP AND BOTTOM OF AVAILABLE MEMORY TO PREVENT SEARCHING OUT
       OF VALID MEMORY BOUNDS */
    (bottom_tag(avail[i])-1)->type = 'B';
    (top_tag(avail[i])+1)->type = 'T';
  }

  /* LINK ALL 64K CHUNKS TOGETHER */
  for (i = 0; i < NUM_BLOCKS-1; i++)
  {
    holepointer(avail[i])->succ = (holelist *)avail[i+1];
    holepointer(avail[i+1])->prev = (holelist *)avail[i];
  }
    holepointer(avail[0])->prev = (holelist *)avail[NUM_BLOCKS-1];
    holepointer(avail[NUM_BLOCKS-1])->succ = (holelist *)avail[0];
    current = avail[0];
  return(1);
}


/*************************************************************************
PROCEDURE K_MALLOC USES ROTATING FIRST-FIT TO FIND A HOLE OF SIZE >=
THAT ASKED FOR (IN BYTES).
*************************************************************************/

void *Safe_malloc(int numbytes)
{

  unsigned *end,*end1,          /* MARKS THE 1st BLOCK SEARCHED (SEARCH
				   TERMINATION MARKER) */
	   *newblock;           /* THE NEW SEGMENT PULLED OUT OF THE HOLE */
  long int numwords;            /* THE MINIMUM SIZE OF THE NEW SEGMENT */
  int intflag;                  /* SAVES THE STATE OF THE INTERRUPT FLAG */

  if (current == NULL)          /* NO ROOM AT THE INN... */
    return (NULL);

  intflag = Mod_int(INT_DISABLE);

  numwords = BYTE_TO_WORD(numbytes);    /* FREE MEMORY STRUCTURE IS UNSIGNED */
  if (numwords < SMALLEST_BLOCK)
    numwords = SMALLEST_BLOCK;
  end = current;                /* THE SEARCH TERMINATION MARKER */
  newblock = NULL;              /* INITIALIZE IN CASE SEARCH FAILS */

  /* FIND USING FIRST-FIT SELECTION METHOD */
  do
  {
    if (bottom_tag(current)->size >= numwords)  /* SEARCH FOR NEXT FIT HOLE */
      extract(current, numwords, &newblock);  /* GET IT IF FOUND */
    current = nextnode(current); /* ELSE CHECK NEXT HOLE */



    end1 = current;             /* THE SEARCH TERMINATION MARKER */

    /* FIND USING FIRST-FIT SELECTION METHOD */
    do
    {
      current = nextnode(current); /* ELSE CHECK NEXT HOLE */
    }
    while ( (current != end1) );        /* OR UNTIL NO MORE HOLES */


  }
  while ( (newblock == NULL)            /* REPEAT UNTIL FOUND A HOLE */
	  && (current != end) );        /* OR UNTIL NO MORE HOLES */


  if (newblock == NULL)
    end = end1;

  Mod_int(intflag);                      /* RESTORE INTERRUPTS */
  return ((void *)newblock);
}


/*************************************************************************
PROCEDURE K_FREE RETURNS THE SEGMENT TO THE FREE_SPACE.
*************************************************************************/

void Safe_free (void *newhole)
{
  unsigned *combo_hole;         /* IF TWO HOLES ARE ADJACENT, CONTAINS
				   THE POINTER TO THE COMBINED HOLE */
  long int combined_size;       /* THE SIZE OF THE COMBO_HOLE */
  int changed;                  /* SET TO 1 IF COALESCING HAPPENED */
  int intflag;                  /* SAVES THE STATE OF THE INTERRUPT FLAG */

  /* IS THIS A VALID SEGMENT? */
  if (bottom_tag(newhole)->type != 'S' ||
      top_tag(newhole)->size != bottom_tag(newhole)->size)
  {
    changed = 0;
    return;
  }

  /* OKAY TO DELETE... */

  intflag = Mod_int(INT_DISABLE);

  bottom_tag(newhole)->type = 'H';
  top_tag(newhole)->type = 'H';

  /* IF ITS THE ONLY HOLE, THIS STUFF'S EASY */
  if (current == NULL)
  {
    current = newhole;
    holepointer(newhole)->prev = (holelist *)newhole;
    holepointer(newhole)->succ = (holelist *)newhole;
  }
  else
  { /* 1+ HOLES ALREADY IN SYSTEM; CHECK THE NEIGHBORS */

    changed = 0;

    if( (combo_hole = above_neighbor(newhole)) != NULL &&
	bottom_tag(combo_hole)->type == 'H')
    {
    /* AN OLD HOLE LIES DIRECTLY ABOVE THE NEW HOLE. COMBINE THEM.*/

      changed = 1;

      /* COMBINED HOLES NO LONGER NEED THE MIDDLE TAGS, SO ADD THEM IN, TOO*/
      combined_size = bottom_tag(newhole)->size + bottom_tag(combo_hole)->size +
		      2*TAG_SIZE;
      bottom_tag(newhole)->size = combined_size;
      top_tag(newhole)->size = combined_size;

	    /* ADD THE NEW LOCATION OF THE HOLE TO THE LIST BY INSERTING THE
	NEW STARTING ADDRESS AND THEN DELETING THE OLD */
      insert (newhole, combo_hole);
      delete (combo_hole);
    }

    if ( (combo_hole = below_neighbor(newhole)) != NULL &&
	 bottom_tag(combo_hole)->type == 'H')
    {
    /* AN OLD HOLE LIES DIRECTLY BELOW THE NEW HOLE. COMBINE THEM.*/
      combined_size = bottom_tag(newhole)->size + bottom_tag(combo_hole)->size +
		      2*TAG_SIZE;
      bottom_tag(combo_hole)->size = combined_size;
      top_tag(combo_hole)->size = combined_size;

      /* IF THE HOLE HAS BEEN COMBINED WITH AN UPPER NEIGHBOR, THEN IT IS
	 IN THE HOLE LIST AND NEEDS TO BE DELETED (THE ENTRY FOR THE UPPER
	 NEIGHBOR WILL SERVE FOR ALL) */
	  if (changed)
	delete(newhole);
      changed = 1;
    }

      if (!changed)
      /* NO NEIGHBORS WHO ARE HOLES. ADD NEW HOLE TO THE LINKED LIST */
	insert(newhole, current);
  }
  Mod_int(intflag);                      /* RESTORE INTERRUPT FLAG */
}


/*************************************************************************
FREE_MALLOC RETURNS ALL OF ITS ALLOCATED SPACE BACK TO THE SYSTEM FREE
SPACE LIST USING 'FREE'.
*************************************************************************/

void Free_malloc()
{
  int i;
  for (i = 0; i < NUM_BLOCKS; i++)
    free(memory[i]);

}



/*************************************************************************
PROCEDURE EXTRACT TAKES THE GIVEN HOLE (THAT IS >= TO THE GIVEN SIZE)
AND EITHER SPLITS IT INTO A SEGMENT OF SIZE size AND A HOLE OF WHATEVER'S
LEFT OR (IF THE SIZES ARE CLOSE) JUST TURNS THE HOLE INTO A SEGMENT.
*************************************************************************/

static void extract (unsigned *holeptr, long int segment_size, unsigned **rtnptr)
{
  unsigned *new_hole,           /* POINTS TO THE HEAD OF THE NEW HOLE */
	   *new_seg;            /*   "    "   "   "   "   "   "  SEGMENT */

  long int hole_size;           /* THE SIZE OF THE NEW HOLE */

  new_seg = holeptr;    /* NEW SEGMENT WILL START WHERE THE HOLE IS NOW */

  /* ARE THE HOLE AND SEGMENT CLOSE TO THE SAME SIZE? */
  if ( abs(segment_size - bottom_tag(holeptr)->size) <= SMALLEST_BLOCK)
  {
    /* YES, SO JUST TURN THE HOLE INTO A SEGMENT */
    bottom_tag(holeptr)->type = 'S';
    top_tag(holeptr)->type = 'S';
    delete(holeptr);            /* AND REMOVE IT FROM THE HOLE LIST */

  /* RETURN THE NEW SEGMENT POINTER TO THE CALLER */

  *rtnptr = holeptr;
  }
  else
  { /* SIZES ARE DIFFERENT - */
  /* SPLIT THE HOLE INTO A SEGMENT AND A HOLE: */

/* 1: CONSTRUCT THE NEW HOLE TAG */

    /* HOLE_SIZE TAKES INTO ACCOUNT THE TWO NEW TAGS; THE FAR TAG FOR THE
       NEW SEGMENT AND THE NEAR TAG FOR THE NEW HOLE. */
    hole_size = bottom_tag(holeptr)->size - segment_size - 2*TAG_SIZE;

    /* CALCULATE THE LOCATION OF THE BEGINNING OF THE NEW HOLE */
    new_hole = holeptr + segment_size + 2*TAG_SIZE;

    /* LABEL ALL OF ITS TAGS */
    bottom_tag(new_hole)->type = 'H';
    bottom_tag(new_hole)->size = hole_size;
    top_tag(new_hole)->size = hole_size;
    top_tag(new_hole)->type = 'H';

    /* INSERT THE NEW HOLE INTO THE HOLE LIST */
    insert (new_hole, holeptr);
    current = new_hole;

/* 2: FIX THE SEGMENT'S BOUNDARY TAGS*/

    bottom_tag(new_seg)->type = 'S';
    bottom_tag(new_seg)->size = segment_size;
    top_tag(new_seg)->type = 'S';
    top_tag(new_seg)->size = segment_size;
    delete (new_seg);              /* REMOVE THE SEGMEMT FROM THE HOLE LIST */


  /* RETURN THE NEW SEGMENT POINTER TO THE CALLER */

  *rtnptr = new_seg;
  } /* END else size of new != old */
} /* end of procedure extract */



/*************************************************************************
**************************************************************************

		FUNCTIONS FOR THE LINKED-LIST "ADT":

	1. insert():
	2. delete();
	3: nextnode();

**************************************************************************
*************************************************************************/


/*************************************************************************
INSERT INSERTS THE NEW HOLE POINTER INTO THE LINKED LIST AFTER THE OLD
HOLE POINTER.
*************************************************************************/

static void insert (unsigned *newloc, unsigned *prevloc)
{
  holelist *succloc;            /* POINTS TO THE OLD HOLE'S SUCCESSOR */

  /* INSERT NEWLOC INTO THE LIST */
  holepointer(newloc)->prev = (holelist *)prevloc;
  holepointer(newloc)->succ = holepointer(prevloc)->succ;

  /* MAKE THE OLD POINTER AND THE SUCCESSOR (IF IT EXISTS) POINT TO THE
     NEW STRUCTURE */
  succloc = holepointer(prevloc)->succ;
  holepointer((unsigned *)succloc)->prev = (holelist *)newloc;
  holepointer((unsigned *)prevloc)->succ = (holelist *)newloc;

}


/*************************************************************************
DELETE REMOVES THE GIVEN NODE FROM THE DOUBLY-LINKED LIST, REDIRECTING THE
CURRENT NODE POINTER (FOR FIRST-FIT) IF WE ARE DELETING THE CURRENT NODE.
*************************************************************************/

static void delete (unsigned *oldhole)
{
  holelist *prevhole,           /* POINTS TO THE PREVIOUS NODE */
	   *succhole;           /* POINTS TO THE SUCCESSOR */

  prevhole = holepointer(oldhole)->prev;
  succhole = holepointer(oldhole)->succ;

  /* ONLY ONE ITEM IN THE LIST? */
  if ( (prevhole == succhole) && (prevhole == (holelist *)oldhole) )
    current = NULL;

  else
  {
    if (current == oldhole)
      current = (unsigned *)prevhole;

    holepointer((unsigned *)prevhole)->succ = succhole;
    holepointer((unsigned *)succhole)->prev = prevhole;
  }

  /* CLEANUP CODE */
  holepointer(oldhole)->prev = NULL;
  holepointer(oldhole)->succ = NULL;
}


/*************************************************************************
NEXTNODE RETURNS THE NEXT NODE IN THE LINKED LIST
*************************************************************************/

static unsigned *nextnode(unsigned *currloc)
{
  if (currloc != NULL)
    return ((unsigned *)(holepointer(currloc)->succ));
  else
    return (currloc);
}




/*************************************************************************

int CPRINTF prints the given string to STDOUT.

*************************************************************************/

#define MAX_PRINT_LENGTH 255

int Cprintf(char *str, ...)
{
  /* don't allocate this from the heap since the message being printed
     might be heap full. */
  static char outstring[MAX_PRINT_LENGTH];
  int count,    /* loop index for printing the string */
	  i,    /* saves the value of the interrupt flag */
	  old0x21;

					/* all variables included in it)    */
  va_list argptr;       /* will point to a (possibly empty) list of     */
			/* variables to be inserted into the string.    */
			/* (for more info, read about the 'C' ellipsis  */
			/* ('...') capabilities )                       */

  /* initialize the output string */
  strset(outstring,'\0');

  /* point to the list of optional arguments */

  va_start(argptr,str);

  /* stuff the variables into the strings */

  old0x21 = inp(0x21);
  outp(0x21,old0x21|3);
  i = Mod_int(INT_ENABLE);
  vsprintf(outstring,str,argptr);
  Mod_int(i);
  outp(0x21,old0x21);

  /* restore the stackpointers correctly */

  va_end(argptr);

  /* print the string on the screen */

  for(count=0; outstring[count] !='\0'; count++)
	   Put_char(outstring[count]);

  return 0;
} /* Cprintf */

int Mod_int(int what)
{
	int old;

	old = _FLAGS & INT_ENABLE;

	if (what == INT_ENABLE)
		asm STI;
	else
		asm CLI;
	return old;
}


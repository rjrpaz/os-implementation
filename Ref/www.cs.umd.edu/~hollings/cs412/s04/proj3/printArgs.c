#include "libuser.h"
#include "libio.h"

#if !defined (NULL)
#define NULL 0
#endif

int Main( int argc , char ** argv )
{
  int i;     	
  for(i=0; i<argc; i++)
    Printf("arg %d = %s\n",i,argv[i]);

  return 1;
}


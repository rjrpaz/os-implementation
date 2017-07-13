#include "libuser.h"
#include "libio.h"


int Main( int argc , char ** argv )
{

  int i,j ;     	/* loop index */

  for (i=0; i < 20; i++) {
    for(j=0;j<20000;j++);
    Printf("2");
  }

  return 0;
}

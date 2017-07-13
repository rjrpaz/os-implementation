int main()
{
  char * p = 0x0000cafe;
  *p = 'A';

/*
 * C: segfault,
 * Cyclone : fails to compile (can't make pointer from integer)
 */
 
  char * p;
  *p = 'A';

/*
 * C: segfault,
 * Cyclone : fails to compile (uninitialized pointer)
 */


  char * p = 0;
  *p = 'A';

/*
 * C: segfault,
 * Cyclone : fails to compile (NULL pointer)
 */


 
  char arr[10];
  arr[11] = 'A';

/*
 * C: goes thru (stabs you in the back later)
 * Cyclone : fails to compile (out of bounds)
 */

  char c;
  char * p = &c;

  *(int *)p = 0xDEADBEEF;

/* 
 * C: goes thru (stabs you in the back later) 
 * Cyclone : fails to compile (incompatible types in cast) 
 */

  char * p = (char *)malloc(sizeof(char));

  free(p);
  *p = 'A';

/*
 * C:  goes thru (stabs you in the back later)
 */

  char @ p = malloc(sizeof(char));
  // free() : not needed
  *p = 'A'; 

/*
 * Cyclone : there's no free(), there's no problem
 */



  return 0;
}

`a id(`a x)
{
  return x;
}

/* 
 * C: can't express polymorphism
 * Cyclone : OK
 */



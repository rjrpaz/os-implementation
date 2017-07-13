/*--------------------------------------------------------------------------
 * Program    :  random.c
 * Author     :  Dr. Joseph K. NG (Hong Kong Baptist University)
 * Description: This is a random generator, taking two arguments:
 *              alpha, max  where alpha is the parameter guiding the 
 *              number generated X falls into the inequality
 *              (0 <= X < 2*alpha ) and max is the number of numbers to be
 *              generated. 
 * To compile : Since random use atof, so we must compile including the math.
 *              Library. Hence, we type, "gcc -o random random.c -lm"
 * To run     : random <alpha> <max_number>
 * Sample run : random 0.5 20
 --------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double random_fun(double);

int main(int argc, char *argv[])
{
 double alpha;
 int i, max;

 if (argc < 3){
    printf("Usage: %s <alpha> <max_number generated>\n", argv[0]);
    exit(1);
 }

 alpha = atof(argv[1]); /* atof() = alphanumeric to float function */
 max = atoi(argv[2]); /* atoi() = alphanumeric to integer function */

 printf("Parameters: range = 0 to %lf  numbers generated = %d\n", 2*alpha, max);

 for (i = 1; i <= max; i++){
     printf("%f\n", random_fun(alpha));
 }
}

double random_fun(double alpha)
{
 static long a = 401;
 static long x = 5;
 static long c = 1793;
 static long m = 32768;

 x = (a * x + c) % m;
 return (alpha * 2.0 * ((double) x) / ((double) m));
}

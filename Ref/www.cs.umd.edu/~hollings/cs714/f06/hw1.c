#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000003L

int main()
{
  long n, k, i;
  int rank, size;
  double x, y;
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  srand(time(NULL));
  n = N;
  k = 0;
  for (i=0; i<n/size; i++) {
    char inside;
    x = rand()/((double)RAND_MAX);
    y = rand()/((double)RAND_MAX);
    if (x * x + y * y < 1.0) {
      inside = 1;
    }
    else {
      inside = 0;
    }
    if (rank == 0) {
      int j;
      if (inside == 1) k = k + 1;
      for (j=1; j<size; j++) {
        MPI_Status status;
        MPI_Recv(&inside, 1, MPI_CHAR, j, 0, MPI_COMM_WORLD, &status);
        if (inside == 1) k = k + 1;
      }
    }
    else {
      MPI_Send(&inside, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }
  }
  if (rank == 0) {
    printf("%f\n", 4.0 * k / ((double)n));
  }
  return 0;
}

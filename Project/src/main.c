#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <mpi.h>
#include "../lib/matrix.h"
#include "../lib/vector.h"
#include "main.h"

int main( int argc, char* argv[]) {
  int ret = 0;
  unsigned int dim_row = 2;
  unsigned int dim_col = 2;
  if( argc > 2) {
    dim_row = atoi(argv[1]);
    dim_col = atoi(argv[2]);
  }

  int rank, size;
  MPI_Init(&argc, &argv);               // starts MPI
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); // get current process id
  MPI_Comm_size(MPI_COMM_WORLD, &size); // get number of processes

  Matrix mtx = { 0, 0, NULL };
  Vector vect = { 0, NULL };

  MPI_Finalize();
  return ret;
}
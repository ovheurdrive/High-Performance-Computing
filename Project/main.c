#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <mpi.h>
#include "matrix.h"
#include "vector.h"
#include "main.h"

int main( int argc, char* argv[]) {
  unsigned int dim_row,dim_col;
  if( argc > 2) {
    dim_row = atoi(argv[1]);
    dim_col = atoi(argv[2]);
  }


  int rank, size;
  MPI_Init(&argc, &argv);               // starts MPI
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); // get current process id
  MPI_Comm_size(MPI_COMM_WORLD, &size); // get number of processes


  Matrix mtx = { 0, 0, NULL };
  build_matrix(dim_row, dim_col, &mtx);
  display_matrix(&mtx);
  free_matrix(&mtx);

  Vector vect = { 0, NULL };
  build_vector(dim_row,&vect);
  display_vector(&vect);
  free_vector(&vect);

  MPI_Finalize();
  return 0;
}
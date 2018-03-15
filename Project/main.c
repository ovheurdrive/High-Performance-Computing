#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <mpi.h>
#include "matrix.h"
#include "vector.h"
#include "main.h"

int main( int argc, char* argv[]) {
  int ret = 0;
  unsigned int dim_row,dim_col;
  if( argc > 2) {
    dim_row = atoi(argv[1]);
    dim_col = atoi(argv[2]);
  }


  int rank, size;
  MPI_Init(&argc, &argv);               // starts MPI
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); // get current process id
  MPI_Comm_size(MPI_COMM_WORLD, &size); // get number of processes


  /*
  *   Test of matrix librairy
  */
  Matrix mtx = { 0, 0, NULL };

  if( ret = build_matrix(dim_col, dim_row, &mtx) != 0) {
    MPI_Abort(MPI_COMM_WORLD, ret);
  }

  randomize_matrix(&mtx,100);
  display_matrix(&mtx, "Matrix random : ");
  free_matrix(&mtx);

  if( ret = read_matrix_from_file(&mtx, "test.txt") != 0) {
    MPI_Abort(MPI_COMM_WORLD, ret);
  }

  display_matrix(&mtx, "Matrix From File : ");
  free_matrix(&mtx);

  /*
  *   Test of vector library
  */
  Vector vect = { 0, NULL };

  if( ret = build_vector(dim_row,&vect) != 0 ) {
    MPI_Abort(MPI_COMM_WORLD, ret);
  }

  randomize_vector(&vect, 20);
  display_vector(&vect, "Vector random : ");
  free_vector(&vect);

  MPI_Finalize();
  return ret;
}
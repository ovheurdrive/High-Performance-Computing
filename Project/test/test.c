#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <mpi.h>
#include "../lib/matrix.h"
#include "../lib/vector.h"

int main( int argc, char* argv[]) {
  int ret = 0;
  unsigned int dim_row = 3;
  unsigned int dim_col = 3;
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

  if( (ret = build_matrix(dim_col, dim_row, &mtx)) != 0) {
    fprintf(stderr, "Out of memory");
    MPI_Abort(MPI_COMM_WORLD, ret);
  }

  randomize_matrix(&mtx,100);
  display_matrix(&mtx, "Matrix random : ");
  free_matrix(&mtx);

  if( (ret = read_matrix_from_file(&mtx, "data/matrix_test.txt")) != 0) {
    fprintf(stderr, "Error when constructing matrix from file in proc %d\n", rank);
    MPI_Abort(MPI_COMM_WORLD, ret);
  }

  display_matrix(&mtx, "Matrix From File : ");
  free_matrix(&mtx);

  /*
  *   Test of vector library
  */
  Vector vect = { 0, NULL };

  if( (ret = build_vector(dim_row,&vect)) != 0 ) {
    fprintf(stderr, "Out of memory");
    free_matrix(&mtx);
    MPI_Abort(MPI_COMM_WORLD, ret);
  }

  randomize_vector(&vect, 20);
  display_vector(&vect, "Vector random : ");
  free_vector(&vect);

  if( (ret = read_vector_from_file(&vect, "data/vector_test.txt")) != 0) {
    fprintf(stderr, "Error when constructing vector from file in proc %d\n", rank);
    free_matrix(&mtx);
    MPI_Abort(MPI_COMM_WORLD, ret);
  }
  display_vector(&vect, "Vector from file : ");
  free_vector(&vect);

  MPI_Finalize();
  return ret;
}
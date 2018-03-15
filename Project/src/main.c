#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <mpi.h>
#include "../lib/matrix.h"
#include "../lib/vector.h"
#include "main.h"

int main( int argc, char* argv[]) {
  int ret = 0;

  int rank, size;
  MPI_Init(&argc, &argv);               // starts MPI
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); // get current process id
  MPI_Comm_size(MPI_COMM_WORLD, &size); // get number of processes

  int root = size - 1;
  Matrix mtx = { 0, 0, NULL };
  Vector vect = { 0, NULL };

  char filename[100];
  snprintf(filename, sizeof(filename), "data/matrix_%d.txt", rank + 1 );

  if( (ret = read_matrix_from_file(&mtx, filename)) != 0) {
    fprintf(stderr, "Error when constructing matrix from file in proc %d\n", rank);
    MPI_Abort(MPI_COMM_WORLD, ret);
  }

  char message[100];
  snprintf(message, sizeof(message), "Matrix from proc %d :", rank);
  display_matrix(&mtx, message);


  snprintf(filename, sizeof(filename), "data/vector_%d.txt", rank + 1 );

  if( (ret = read_vector_from_file(&vect, filename)) != 0) {
    fprintf(stderr, "Error when constructing vector from file in proc %d\n", rank);
    MPI_Abort(MPI_COMM_WORLD, ret);
  }

  snprintf(message, sizeof(message), "Vector from proc %d :", rank);
  display_vector(&vect, message);


  MPI_Finalize();
  return ret;
}
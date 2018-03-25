#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <errno.h>
#include "../lib/matrix.h"
#include "../lib/vector.h"
#include "../lib/operations.h"
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

  // Initiate Matrix

  if( (ret = read_matrix_from_file(&mtx, "data/matrix.txt")) != 0) {
    fprintf(stderr, "Error when constructing matrix from file in proc %d\n", rank);
    MPI_Abort(MPI_COMM_WORLD, ret);
  }

  char message[100];
  snprintf(message, sizeof(message), "Matrix from proc %d :", rank);
  display_matrix(&mtx, message);

  // Initiate Vector

  if( (ret = read_vector_from_file(&vect, "data/vector.txt")) != 0) {
    fprintf(stderr, "Error when constructing vector from file in proc %d\n", rank);
    MPI_Abort(MPI_COMM_WORLD, ret);
  }

  // Check if the number of process is inferior to the size of the data

  if( size > vect.size ) {
    if( rank == root)
      fprintf(stderr, "Error, too many processes regarding the vector size\n");
    MPI_Abort(MPI_COMM_WORLD, EINVAL);
  }

  snprintf(message, sizeof(message), "Vector from proc %d :", rank);
  display_vector(&vect, message);

  
  /*
  *  Scattering the vector and the matrices in the processors :
  *  We can either use MPI_Scatter to distribute the Data,
  *  Or we can split the matric and the vector beforhand in different files
  *  that we load in the corresponding processors
  */



  Vector result;
  ret = product_vector_matrix( &result, &vect, &mtx );
  if( ret != 0 ) return ret;
  display_vector(&result, "Result");

  free_matrix(&mtx);
  free_vector(&vect);

  MPI_Finalize();
  return ret;
}
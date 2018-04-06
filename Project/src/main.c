#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <errno.h>
#include <stdbool.h>
#include "../lib/matrix.h"
#include "../lib/vector.h"
#include "../lib/operations.h"
#include "main.h"

int main( int argc, char* argv[]) {
  int ret = 0;

  // MPI Initialization

  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int root = size - 1;
  int iterations = 0;
  Matrix mtx = { 0, 0, NULL };
  Vector vect = { 0, NULL };

  // Read Metadata
  
  FILE* metadata_file = fopen("data/metadata.txt", "r");
  int rows, col, local_rows, first_row;
  if( fscanf(metadata_file, "%d %d", &rows, &col) < 2 ){
    fclose(metadata_file);
    MPI_Abort(MPI_COMM_WORLD, EINVAL);
  }
  fclose(metadata_file);

  // Check if the number of process is inferior to the size of the data

  if( size > rows ) {
    if( rank == root)
      fprintf(stderr, "Error, too many processes regarding the vector size\n");
      MPI_Abort(MPI_COMM_WORLD, EINVAL);
  }

  // Split vector and matrix according to number of process
  
  local_rows = rows / size;
  first_row = rank * local_rows;
  if( rank < local_rows % size ) {
    local_rows++;
    first_row += rank;
  }
  else {
    first_row += local_rows % size;
  }

  // Initiate Matrix

  if( (ret = read_matrix_from_file(&mtx, "data/matrix.txt", first_row, local_rows, col)) != 0) {
    fprintf(stderr, "Error when constructing matrix from file in proc %d\n", rank);
    goto fail;
  }

  char message[100];
  snprintf(message, sizeof(message), "Matrix from proc %d :", rank);
  display_matrix(&mtx, message);

  // Initiate Vector

  if( (ret = read_vector_from_file(&vect, "data/vector.txt", first_row, local_rows)) != 0) {
    fprintf(stderr, "Error when constructing vector from file in proc %d\n", rank);
    goto fail;
  }

  snprintf(message, sizeof(message), "Vector from proc %d :", rank);
  display_vector(&vect, message);

  // We create a boolean array to check if a proc needs to continu iterating and a global boolean
  
  bool run = true;
  bool* continue_iterate = malloc(size*sizeof(bool));
  for(int i = 0; i < size; i++ ) {
    continue_iterate[i] = true;
  }

  // We create local and global result vectors of the right size for jacobi
  
  Vector local_result = { 0, NULL }; // local result 
  Vector global_result = { 0, NULL }; // global vector for product
  if( (ret = build_vector(local_rows, &local_result)) != 0 ) {
    goto fail;
  }
  if( (ret = build_vector(rows, &global_result)) != 0 ) {
    goto fail;
  }

  // Main Jacobi Loop
  
  while( run && (iterations < 1)) {
    display_vector(&local_result, "");
    display_vector(&global_result, "");
    iterations++;
  }
  

  Vector result;
  ret = product_vector_matrix( &local_result, &global_result, &mtx );
  if( ret != 0) {
    fprintf(stderr, "Error when performing operation\n");
    goto fail;
  }
  display_vector(&result, "Result");

  free_matrix(&mtx);
  free_vector(&vect);

  MPI_Finalize();
  return ret;


fail:
  free_matrix(&mtx);
  free_vector(&vect);
  free_vector(&local_result);
  free_vector(&global_result);
  MPI_Abort(MPI_COMM_WORLD, ret);
}
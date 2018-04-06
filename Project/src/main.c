// Standard Libraries

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

// Utilities 

#include <math.h>
#include <mpi.h>
#include <errno.h>

// Libs

#include "../lib/matrix.h"
#include "../lib/vector.h"
#include "main.h"

int main( int argc, char* argv[]) {
  int ret = 0;

  // MPI Initialization

  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Request request;
  MPI_Status status;

  // Parsin Arguments

  bool verbose = false;
  int arg;
  while ( (arg = getopt(argc, argv, "vh?")) != -1 )
    switch ( arg )
    {
      case 'v':
        verbose = true;
        break;
      case 'h':
      case '?':
        usage(argv[0]);
        break;
    }

  // Init Variables

  int root = size - 1;
  int iterations = 0;
  Matrix mtx = { 0, 0, NULL };
  Vector vect = { 0, NULL };
  bool* continue_iterate = NULL;

  // Read Metadata
  
  FILE* metadata_file = fopen("data/metadata.txt", "r");
  int rows, col, local_rows, first_row, r;
  if( metadata_file ) {
    if( fscanf(metadata_file, "%d %d", &rows, &col) < 2 ){
      fclose(metadata_file);
      MPI_Abort(MPI_COMM_WORLD, EINVAL);
    }
    fclose(metadata_file);
  }
  else {
    MPI_Abort(MPI_COMM_WORLD, ENOENT);
  }

  // Check if the number of process is inferior to the size of the data

  if( size > rows ) {
    if( rank == root)
      fprintf(stderr, "Error, too many processes regarding the vector size\n");
      MPI_Abort(MPI_COMM_WORLD, EINVAL);
  }

  // Split vector and matrix according to number of process
  
  local_rows = rows / size;
  r = rows % size;
  first_row = rank * local_rows;
  if( rank < r ) {
    local_rows++;
    first_row += rank;
  }
  else {
    first_row += r;
  }  

  // Initiate Matrix

  if( (ret = read_matrix_from_file(&mtx, "data/matrix.txt", first_row, local_rows, col)) != 0) {
    fprintf(stderr, "Error when constructing matrix from file in proc %d\n", rank);
    goto fail;
  }

  if( verbose ) {
    char message[100];
    snprintf(message, sizeof(message), "Matrix from proc %d :", rank);
    display_matrix(&mtx, message);
  }

  // Initiate Vector

  if( (ret = read_vector_from_file(&vect, "data/vector.txt", first_row, local_rows)) != 0) {
    fprintf(stderr, "Error when constructing vector from file in proc %d\n", rank);
    goto fail;
  }

  if( verbose ) {
    char message[100];
    snprintf(message, sizeof(message), "Vector from proc %d :", rank);
    display_vector(&vect, message);
  }

  // We create a boolean array to check if a row needs to continue iterating and a global run boolean
  
  bool run = true;
  continue_iterate = malloc(local_rows*sizeof(bool));
  for(int i = 0; i < local_rows; i++ ) {
    continue_iterate[i] = true;
  }

  // We create local and global result vectors of the right size for jacobi
  
  Vector local_result = { 0, NULL }; // local result 
  Vector global_result = { 0, NULL }; // global vector for product
  if( (ret = build_vector(local_rows, &local_result)) != 0 ) {
    goto fail;
  }
  if( (ret = build_vector(col, &global_result)) != 0 ) {
    goto fail;
  }

  // Main Jacobi Loop
  
  while( run && (iterations < 50000)) {


    // First we need to fill the global vector with the result from the other proc
    for( int i = 0; i < col; i++ ) {

      // We send the value when we reach the i corresponding the the vector row

      if ( i >= first_row && i < first_row + local_rows) {

        // we first copy the values in the right place in the global vector

        global_result.vector[i] = local_result.vector[i-first_row];

        // We send the values to the other processors

        for( int p = 0; p < size; p++) {
          if( p != rank ) {
            MPI_Isend(&global_result.vector[i], 1, MPI_DOUBLE, p, 0, MPI_COMM_WORLD, &request);
          }
        }
      }
      else {

        // We receive the values from the other processors
        int remaining = col % size;
        int processor;
        if( i < remaining * ((col / size) + 1)) {
          processor = i / ((col / size) + 1 );
        }
        else {
          processor = i - remaining*(rows / size + 1 );
          processor = remaining + processor/(rows/size);
        }

        MPI_Irecv(&global_result.vector[i], 1, MPI_DOUBLE, processor, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status); 
      }
    }

    // Calculate next iteration of the local result

    for( int i = 0; i < local_rows; i++ ) {

      // We check if the row needs to be iterated

      if(continue_iterate[i] == false) continue;

      // Jacobi recurrence formula

      double local_vect_matrix_sum = 0;
      for( int j = 0; j < col; j++ ) {
        if( i + first_row != j) local_vect_matrix_sum += mtx.matrix[i][j] * global_result.vector[j];
      }

      local_result.vector[i] = ( 1 / mtx.matrix[i][i + first_row]) * ( vect.vector[i] - local_vect_matrix_sum);

      // Residual analysis

      double error = fabs(local_result.vector[i] - global_result.vector[first_row + i]);

      if( verbose ) {
        printf("Residue on processor %d row %d : %lf\n", rank, i+first_row, error);
      }

      if( error < 1e-6 ){
        continue_iterate[i] = false;
      }
    }

    // Test if the processor still have some rows to iterate

    int local_continue_to_process = false;
    int iterator = 0;
    while( iterator < local_rows && local_continue_to_process == false ) {
      local_continue_to_process |= continue_iterate[iterator];
      iterator++;
    }

    // root process checking if we stop the loop globally

    if( rank == root ) {
      bool continue_global = local_continue_to_process;

      for( int p = 0; p < root; p++) {
        bool continue_from_proc_p = false;
        MPI_Irecv(&continue_from_proc_p, 1, MPI_C_BOOL, p, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        continue_global |= continue_from_proc_p;
      }

      // Receives continues
      for (int p = 0; p < root; p++) {
        MPI_Isend(&continue_global, 1, MPI_C_BOOL, p, 0, MPI_COMM_WORLD, &request);
      }
      run = continue_global;
    }
    else {
      MPI_Isend(&local_continue_to_process, 1, MPI_C_BOOL, root, 0, MPI_COMM_WORLD, &request);
      MPI_Irecv(&run, 1, MPI_C_BOOL, root, 0, MPI_COMM_WORLD, &request);
      MPI_Wait(&request, &status);
    }
    iterations++;
  }

  if (rank == root)
  {
    printf("Converged in %d iterations\nSolution found:\n", iterations);
    display_vector(&global_result, "Result : ");

    if( verbose ) {
      Vector test_rest = { 0, NULL };
      Matrix global_matrix = { 0, 0, NULL };
      read_matrix_from_file(&global_matrix, "data/matrix.txt", 0, rows, col);
      build_vector(col, &test_rest);
      product_vector_matrix(&test_rest, &global_result, &global_matrix);
      display_vector(&test_rest, "Injection of solution into equation : ");
      free_vector(&test_rest);
      free_matrix(&global_matrix);
    }
  }

  free(continue_iterate);
  free_vector(&local_result);
  free_vector(&global_result);
  free_matrix(&mtx);
  free_vector(&vect);

  MPI_Finalize();
  return ret;


fail:
  free(continue_iterate);
  free_vector(&local_result);
  free_vector(&global_result);
  free_matrix(&mtx);
  free_vector(&vect);
  MPI_Abort(MPI_COMM_WORLD, ret);
}

int product_vector_matrix(Vector* dest, Vector* vect, Matrix* mtx) {
  int ret = 0;
  if( (vect->size != mtx->col) || (dest->size != mtx->rows) ) {
    return -1;
  }
  for( int i = 0; i < mtx->rows; i++ ) {
    for( int j = 0; j < mtx->col; j++ ) {
      dest->vector[i] += mtx->matrix[i][j] * vect->vector[j];
    }
  }
  return ret;
}

static void usage ( const char *prog )
{
  fprintf(stderr, "Usage: mpirun -n n_proc %s [-h] [-v]\n\n", prog);
  MPI_Abort(MPI_COMM_WORLD, 1);
}

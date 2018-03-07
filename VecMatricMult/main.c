#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int** create_matrix(int m_size) {
  int** matrix = malloc( (m_size) * sizeof(int*));
  for( int i=0; i<m_size; i++ ) {
    matrix[i] = malloc( ( m_size) * sizeof(int));
  }
  return matrix;
}

int** fill_matrix(int **matrix, int m_size) {
  for( int i=0; i<m_size; i++ ) {
    for( int j=0; j<m_size; j++ ) {
      matrix[i][j] = i + j ;
    }
  }
  return matrix;
}

void display_matrix(int** matrix, int m_size) {
  for(int i=0; i<m_size; i++) {
    printf("( ");
    for(int j=0; j<m_size; j++) {
      printf("%d ", matrix[i][j]);
    }
    printf(")\n");
  }
  printf("\n");
}

void free_matrix(int** matrix, int m_size) {
  for( int i=0; i<m_size; i++ ) {
    free(matrix[i]);
  }
  free(matrix);
}

int main(int argc, char* argv[]) {
  int m_size = 5;
  if( argc > 1) {
    m_size = atoi(argv[1]);
  }

  int rank, size;
  MPI_Init(&argc, &argv);               // starts MPI
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); // get current process id
  MPI_Comm_size(MPI_COMM_WORLD, &size); // get number of processes
  
  int** matrix = create_matrix(m_size);
  matrix = fill_matrix(matrix,m_size);
  
  int* vector = malloc( m_size * sizeof(int));
  for(int k=0; k<m_size; k++) {
    vector[k] = (k + 3)/2;
  }
  int** sub_matrix;
  int sub_size = m_size / size;
  printf("Computing in proc %d\n", rank);
  if( rank == 0 ) {
    for( int i = 0; i< sub_size; i++) {
      for( int j=0; j<m_size; j++) {
        matrix[i][j] = matrix[i][j] * vector[i];
      }
    }

    int** matrix_from_sub_proc;

    for( int k = 1; k<size; k++) {
      printf("Receiving data from proc %d\n", k);
      MPI_Recv(matrix_from_sub_proc, m_size, MPI_INT, k, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      if( rank == size -1){
        for(int i=k*sub_size; i<k*(sub_size+1); i++ ) {
          matrix[i] = matrix_from_sub_proc[i];
        }
      }
      else {
        for(int i=k*sub_size; i<m_size; i++ ) {
          matrix[i] = matrix_from_sub_proc[i];
        }
      }
    }
    display_matrix(matrix,m_size);
  }
  else if( rank == size-1 ) {
    printf("Last proc");  
    for(int i=rank*sub_size; i<m_size; i++ ) {
      for( int j=0; j<m_size; j++) {
        matrix[i][j] = matrix[i][j] * vector[i];
      }
    }
    MPI_Send(matrix, m_size, MPI_INT, 0, 1,  MPI_COMM_WORLD);
  }
  else {
    printf("Not last proc");
    for(int i=rank*sub_size; i<rank*(sub_size+1); i++ ) {
      for( int j=0; j<m_size; j++) {
        matrix[i][j] = matrix[i][j] * vector[i];
      }
    }
    MPI_Send(matrix, m_size, MPI_INT, 0, 1,  MPI_COMM_WORLD);
  }

  free_matrix(matrix,m_size);
  free(vector);
  MPI_Finalize();
  return 0;
}
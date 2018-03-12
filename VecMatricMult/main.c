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
      matrix[i][j] = (int)rand() / (int)(RAND_MAX/50) ;
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
  printf("\n\n");
}

void display_vector(int* vector, int m_size) {
  printf(" vector : ( ");
  for(int j=0; j<m_size; j++) {
    printf("%d ", vector[j]);
  }
  printf(")\n");
}

void free_matrix(int** matrix, int m_size) {
  for( int i=0; i<m_size; i++ ) {
    free(matrix[i]);
  }
  free(matrix);
}

int *create_and_fill_vector(int v_size, int seed){
  int* vector = malloc(v_size * sizeof(int));
  for( int i = 0; i<v_size; i++){
    vector[i] = (int)rand() / (int)(RAND_MAX/seed);
  }
  return vector;
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
  
  int *global_result = calloc( m_size, sizeof(int));
  int *global_vector = malloc( m_size * sizeof(int));

  int sub_size = m_size / size;

// Generating local vectors for each process

  int* local_vector;
  int local_num_rows;
  if(rank == size-1) {
    display_matrix(matrix, m_size);
    local_num_rows = m_size - (rank)*sub_size;
  }
  else {
    local_num_rows = sub_size;
  }
  local_vector = create_and_fill_vector(local_num_rows, (int)rand()/(int)(RAND_MAX/10) * (rank+1));
  int* local_result = calloc(local_num_rows, sizeof(int));
  

// Sending local vectors to every process
  
  for(int k=0; k<size; k++) {
    if( k == rank ) {
      for(int i=0; i<local_num_rows; i++){
        global_vector[k*sub_size+i] = local_vector[i];
      }
      continue;
    }

    int tmp_size;
    MPI_Send(&local_num_rows, 1, MPI_INT, k, 0, MPI_COMM_WORLD);
    MPI_Send(local_vector, local_num_rows, MPI_INT, k, 1, MPI_COMM_WORLD);
    MPI_Recv(&tmp_size, 1, MPI_INT, k, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int tmp_vector[tmp_size];
    MPI_Recv(tmp_vector, tmp_size, MPI_INT, k, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    for(int i=0; i<tmp_size; i++){
      global_vector[k*sub_size+i] = tmp_vector[i];
    }
  }

// Calculating local matrix vector multiplication
  if( rank == size - 1){
    
    for( int i = 0; i<local_num_rows; i++) {
      for( int j = 0; j<m_size; j++) {
        global_result[rank*sub_size+i] += matrix[rank*sub_size+i][j] * global_vector[j];
      }
    }
    for( int k = 0; k<size-1; k++) {
      int tmp_size;
      MPI_Recv(&tmp_size, 1, MPI_INT, k, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      int tmp_result[tmp_size];
      MPI_Recv(tmp_result, tmp_size, MPI_INT, k, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      for( int i = 0; i<tmp_size; i++) {
        global_result[k*sub_size+i] = tmp_result[i];
      }
    }
    display_vector(global_vector, m_size);
    display_vector(global_result,m_size);
  }
  else {
    for( int i = 0; i<local_num_rows; i++) {
      for( int j = 0; j<m_size; j++) {
        local_result[i] += matrix[rank*sub_size+i][j] * global_vector[j];
      }
    }
    MPI_Send(&local_num_rows, 1, MPI_INT, size-1, 2, MPI_COMM_WORLD);
    MPI_Send(local_result, local_num_rows, MPI_INT, size-1, 3, MPI_COMM_WORLD);
  }

  free_matrix(matrix,m_size);
  free(local_vector);
  free(global_vector);
  free(global_result);
  free(local_result);
  MPI_Finalize();
  return 0;
}
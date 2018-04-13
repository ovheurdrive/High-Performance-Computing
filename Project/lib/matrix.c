#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include "matrix.h"

int build_matrix( const unsigned int dimension_columns, const unsigned int dimension_rows, Matrix* mtx){
  mtx->col = dimension_columns;
  mtx->rows = dimension_rows;
  mtx->matrix = malloc( mtx->rows * sizeof(double*));
  if(mtx->matrix == NULL){
    return ENOMEM;
  }
  for( int i = 0; i < mtx->rows; i++ ) {
    mtx->matrix[i] = calloc(mtx->col, sizeof(double));
    if(mtx->matrix[i] == NULL){
      mtx->rows = i;
      free_matrix(mtx);
      return ENOMEM;
    }
  }

  return 0;
}

void display_matrix(Matrix* mtx, const char* message) {
  printf("%s\n", message);
  for( int i = 0; i < mtx->rows; i++){
    printf("( ");
    for( int j = 0; j < mtx->col; j++) {
      printf("%lf ", mtx->matrix[i][j]);
    }
    printf(")\n");
  }
  printf("\n");
}

void free_matrix(Matrix* mtx) {
  for( int i = 0; i < mtx->rows; i++ ) {
    free(mtx->matrix[i]);
  }
  free(mtx->matrix);
  mtx->rows = 0;
  mtx->col = 0;
  mtx->matrix = NULL;
}

void randomize_matrix(Matrix* mtx, int max) {
  time_t t;
  srand((unsigned) time(&t));
  for( int i = 0; i < mtx->rows; i++ ) {
    for( int j = 0; j < mtx->col; j++ ) {
      mtx->matrix[i][j] =  (double)(rand() % max);
    }
  }
}

int read_matrix_from_file(Matrix* mtx, char* filepath, const unsigned int first_row, const unsigned int num_rows, const unsigned int num_col) {
  int ret = 0;
  int current_row = 0;
  char buffer[10000];
  FILE* file = fopen(filepath, "r");

  if( file ) {
    // skip rows until we reach first row
    // rows are numeroted from the first line of the matrix as we skip
    // the size before.
    while( current_row < first_row ){
      fgets(buffer, 10000, file);
      char* eol = strchr(buffer, '\n');
      if( eol == NULL){
        return EINVAL;
      }
      current_row++;
    }

    if( (ret = build_matrix(num_col, num_rows, mtx)) != 0 ) {
      return ret;
    }
    for( int i = 0; i < mtx->rows; i++ ) {
      for( int j = 0; j < mtx->col; j++ ) {
        if( fscanf(file, "%lf ", &mtx->matrix[i][j]) < 1 ) {
          free_matrix(mtx);
          return EINVAL;
        }
      }
    }
    fclose(file);
  }
  else {
    return ENOENT;
  }
  return ret;
}
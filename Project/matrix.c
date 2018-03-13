#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include "matrix.h"

int build_matrix( const unsigned int dimension_columns, const unsigned int dimension_rows, Matrix* mtx){
  mtx->col = dimension_columns;
  mtx->rows = dimension_rows;
  mtx->matrix = malloc( mtx->rows * sizeof(int*));
  if(mtx->matrix == NULL){
    return ENOMEM;
  }
  for( int i = 0; i < mtx->rows; i++ ) {
    mtx->matrix[i] = calloc(mtx->col, sizeof(int));
    if(mtx->matrix[i] == NULL){
      mtx->rows = i;
      free_matrix(mtx);
      return ENOMEM;
    }
  }

  return 0;
}

void display_matrix(Matrix* mtx) {
  for( int i = 0; i < mtx->rows; i++){
    printf("( ");
    for( int j = 0; j < mtx->col; j++) {
      printf("%d ", mtx->matrix[i][j]);
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
      mtx->matrix[i][j] =  (int)rand() % max;
    }
  }
}
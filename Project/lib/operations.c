#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "matrix.h"
#include "vector.h"
#include "operations.h"

int product_vector_matrix(Vector* dest, Vector* vect, Matrix* mtx) {
  int ret = 0;
  if( (ret = build_vector(vect->size, dest)) != 0 ) {
    return ret;
  }
  for( int i = 0; i < mtx->rows; i++ ) {
    for( int j = 0; j < mtx->col; j++ ) {
      dest->vector[i] += mtx->matrix[i][j] * vect->vector[j];
    }
  }
  return ret;
}
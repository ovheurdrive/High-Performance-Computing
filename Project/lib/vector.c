#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include "vector.h"

int build_vector( const unsigned int size, Vector* vect) {
  vect->size = size;
  vect->vector = calloc(vect->size, sizeof(double));
  if(vect->vector == NULL){
    return ENOMEM;
  }
  return 0;
}

void display_vector(Vector* vect, const char* message) {
  printf("%s ( ", message);
  for(int i = 0; i < vect->size; i++) {
    printf("%lf ", vect->vector[i]);
  }
  printf(")\n");
}

void free_vector(Vector* vect){
  free(vect->vector);
  vect->vector = NULL;
  vect->size = 0;
}

void randomize_vector(Vector* vect, int max) {
  time_t t;
  srand((unsigned) time(&t));
  for( int i = 0; i < vect->size; i++ ) {
    vect->vector[i] = (double)(rand() % max);
  }
}

int read_vector_from_file(Vector* vect, char* filepath, const unsigned int first_row, const unsigned int size) {
  int ret = 0;
  int current_row = 0;
  char buffer[100];
  FILE* file = fopen(filepath, "r");
  
  if( file ) {
    // skip rows until we reach first row
    // rows are numeroted from the first line of the matrix as we skip
    // the size before.
    while( current_row < first_row ){
      fgets(buffer, 100, file);
      if( strchr(buffer, '\n') == NULL){
        return EINVAL;
      }
      current_row++;
    }

    if( (ret = build_vector(size, vect)) != 0) {
      return ret;
    }
    for( int i = 0; i < vect->size; i++ ) {
      if( fscanf(file, "%lf \n", &vect->vector[i]) < 1 ) {
        free_vector(vect);
        vect->size = 0;
        return EINVAL;
      }
    }
    fclose(file);
  }
  else {
    return ENOENT;
  }
  return ret;
}
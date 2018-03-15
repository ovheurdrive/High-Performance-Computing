#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include "vector.h"

int build_vector( const unsigned int size, Vector* vect) {
  vect->size = size;
  vect->vector = calloc(vect->size, sizeof(int));
  if(vect->vector == NULL){
    return ENOMEM;
  }
  return 0;
}

void display_vector(Vector* vect, const char* message) {
  printf("%s ( ", message);
  for(int i = 0; i < vect->size; i++) {
    printf("%d ", vect->vector[i]);
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
    vect->vector[i] = (int)rand() % max;
  }
}

int read_vector_from_file(Vector* vect,char* filepath) {
  int size;
  FILE* file = fopen(filepath, "r");
  if( file ) {
    if( fscanf(file, "%d \n\n", &size) < 1 ) {
      return EINVAL;
    }
    if( size < 0 ) {
      return EINVAL;
    }
    build_vector(size, vect);
    for( int i = 0; i < vect->size; i++ ) {
      if( fscanf(file, "%d \n", &vect->vector[i]) < 1 ) {
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
  return 0;
}
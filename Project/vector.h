typedef struct Vector{
  unsigned int size;
  int* vector;
}Vector;

int build_vector( const unsigned int size, Vector* vect);
void display_vector(Vector* vect);
void free_vector(Vector* vect);
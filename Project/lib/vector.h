typedef struct Vector{
  unsigned int size;
  double* vector;
}Vector;

int build_vector( const unsigned int size, Vector* vect);
void display_vector(Vector* vect, const char* message);
void free_vector(Vector* vect);

void randomize_vector(Vector* vect, int max);

int read_vector_from_file(Vector* vect, char* filepath, const unsigned int first_row, const unsigned int size);
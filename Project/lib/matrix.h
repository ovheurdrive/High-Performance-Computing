typedef struct Matrix{
  unsigned int col;
  unsigned int rows;
  double **matrix; 
}Matrix;

int build_matrix( const unsigned int dimension_columns, const unsigned int dimension_rows, Matrix* mtx);
void display_matrix(Matrix* mtx, const char* message);
void free_matrix(Matrix* mtx);

void randomize_matrix(Matrix* mtx,int max);

int read_matrix_from_file(Matrix* mtx, char* filepath);
typedef struct Matrix{
  unsigned int col;
  unsigned int rows;
  int **matrix; 
}Matrix;

int build_matrix( const unsigned int dimension_columns, const unsigned int dimension_rows, Matrix* mtx);
void display_matrix(Matrix* mtx);
void free_matrix(Matrix* mtx);
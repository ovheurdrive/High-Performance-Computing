#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

double f(double x){
  return 4/(1+x*x);
}

double trapeze(double a, double b, int n){
  double h = (b - a)/n;
  double integral = (f(a) + f(b))/2;
  for( int k = 1; k < n; k++){
    integral += f(a + k * h);
  }
  integral *= h;
  printf("%lf, %lf, %lf, %d\n", integral, a, b, n);
  return integral;
}

int main(int argc, char* argv[]){
  double a = 0, b = 1;
  int n = 24000;  // default subdivision value
  double integral;
  if ( argc > 1){
    n = atoi(argv[1]);
  }
  double pas = (b-a)/n;

  int rank, size;
  MPI_Init(&argc, &argv);               // starts MPI
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); // get current process id
  MPI_Comm_size(MPI_COMM_WORLD, &size); // get number of processes

  switch(rank){
    case 0:
      integral = trapeze(a, a + pas * n / size, n / size );
      double int_from_sub_proc;

      for( int k = 1; k < size; k++){
        printf("Receving data from proc %d\n", k);
        MPI_Recv(&int_from_sub_proc, 1, MPI_DOUBLE, k, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        integral += int_from_sub_proc;
      }
      printf("Integral value : %lf, with error %.3e\n", integral, M_PI - integral);
    break;
    default:
      printf("Calculating process %d\n", rank);
      double loca = a + rank * pas * n / size;
      double locb = loca + pas * n / size;

      double inte = trapeze(loca, locb, n / size );
      MPI_Send(&inte, 1, MPI_DOUBLE, 0, 0,  MPI_COMM_WORLD);
    break;
  }

  MPI_Finalize();
}
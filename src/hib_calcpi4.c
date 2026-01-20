#include <stdio.h>
#include <openacc.h>
#include <mpi.h> 
#include <omp.h> 

int main(int argc, char *argv[] ) {  /* hib_calcpi4.c */
static long num_passos = 10000000000;
double passo = 1.0 / (double) num_passos;
double soma=0.0, pi = 0.0, soma_global = 0.0, inicio, fim;
int ranque, num_procs;
int thr_status;

  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &thr_status);
  if (thr_status != MPI_THREAD_FUNNELED) {
      printf("Erro ao iniciar no modo MPI_THREAD_FUNNELED\n");
      return(-1);
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &ranque);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  inicio = omp_get_wtime();
  soma = 0.0;
  #pragma acc parallel loop reduction(+:soma)
  for (long int i = ranque; i <= num_passos; i += num_procs) {
       double x = (i + 0.5) * passo;
       soma += 4.0 / (1.0 + x * x);
    }
  MPI_Reduce(&soma, &soma_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  if (ranque == 0) { // Processo 0 calcula o valor final de Pi
      pi = passo * soma_global; 
      fim = omp_get_wtime();
      printf("pi = %2.15f, %ld passos, computados em %lf segundos\n", pi, num_passos, fim-inicio);
  }  
  MPI_Finalize();
  return 0;
}

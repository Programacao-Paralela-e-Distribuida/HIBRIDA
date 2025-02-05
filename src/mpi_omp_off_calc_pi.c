#include <stdio.h>
#include "mpi.h"
#include <omp.h>
static long num_passos = 10000000000;
double passo;
int main(int argc, char *argv[])
{
  int ranque, numprocs, provided;
  double x, pi, soma = 0.0, soma_global = 0.0;
  double inicio, tempo;
  // Inicia o MPI com suporte para threads
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
  if (provided < MPI_THREAD_FUNNELED) {
    printf("Nível de suporte para threads não é suficiente!\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &ranque); // O rank do processo
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs); // O número de processos
  passo = 1.0 / (double)num_passos;
  inicio = omp_get_wtime(); // Tempo de início da execução
// Offloading com OpenMP para o acelerador (GPU), com paralelismo dentro do processo
#pragma omp target data map(tofrom:soma) map(to:numprocs, num_passos, ranque, passo) map(alloc:x) device(1)
#pragma omp target teams distribute parallel for reduction(+:soma) 
  for (long int i = ranque; i < num_passos; i += numprocs) { // Saltos de acordo com o número de processos
    double x = (i + 0.5) * passo;
    soma += 4.0 / (1.0 + x * x);
  }
  // MPI_Reduce para somar os resultados de todos os processos MPI
  MPI_Reduce(&soma, &soma_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  if (ranque == 0) {
    pi = passo * soma_global; // Só o processo 0 calcula o valor final de Pi
    tempo = omp_get_wtime() - inicio;
    printf("pi = %3.15f, %ld passos, computados em %lf segundos\n", pi, num_passos, tempo);
  }  
  MPI_Finalize(); // Finaliza o MPI
  return 0;
}

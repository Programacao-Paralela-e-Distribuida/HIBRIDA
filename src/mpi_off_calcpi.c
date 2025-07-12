#include <stdio.h>
#include <mpi.h>
#include <omp.h>
const long num_passos = 10000000000;

int main(int argc, char *argv[]) {   /* mpi_off_calcpi.c */
  int meu_ranque, num_procs, fornecido;
  double soma = 0.0, soma_global = 0.0;
  double passo = 1.0 / (double)num_passos;
  double inicio, fim;
  
  // Inicia o MPI com suporte para threads
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &fornecido);
  if (fornecido < MPI_THREAD_FUNNELED) {
      printf("Nível de suporte para threads não é suficiente!\n");
      MPI_Abort(MPI_COMM_WORLD, 1);
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque); 
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs); 
  
  // Início da execução
  inicio = omp_get_wtime(); 
  // Offloading com OpenMP para o acelerador (GPU)
  #pragma omp target data map(tofrom:soma) map(to:num_procs, num_passos, meu_ran
que, passo) device(1)
  #pragma omp target teams distribute parallel for reduction(+:soma)
  // Saltos de acordo com o número de processos (MPI)
  for (long int i = meu_ranque; i < num_passos; i += num_procs) { 
       double x = (i + 0.5) * passo;
       soma += 4.0 / (1.0 + x * x);
  }
  
  // MPI_Reduce soma os resultados parciais dos processos MPI
  MPI_Reduce(&soma, &soma_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  double pi = passo * soma_global;
  // Processo raiz (0) finaliza e imprime os resultadr]os
  if (meu_ranque == 0) {  
      fim = omp_get_wtime();
      printf("Valor calculado de Pi: %2.15f em %lf segundos\n", pi, fim-inicio);
  } 

  // Finaliza o MPI
  MPI_Finalize();
  return 0;
}

#include <stdio.h>
#include <mpi.h>
#include <omp.h>
const long num_passos = 10000000000;

int main(int argc, char *argv[]) { /* hib_calcpi3.c */
  int meu_ranque, num_procs, fornecido;
  double soma_local = 0.0, soma_global = 0.0;
  double inicio, fim, soma_cpu = 0.0, soma_gpu = 0.0;
  double passo = 1.0 / (double)num_passos; 
  
  // Inicia o MPI com suporte para threads
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &fornecido);
  if (fornecido < MPI_THREAD_FUNNELED) {
      printf("Nível de suporte para threads não é suficiente!\n");
      MPI_Abort(MPI_COMM_WORLD, 1);
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque); 
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs); 
  
  // Divisão da carga entre GPU e CPUs
  long int split_point = (int)(num_passos * .7); // 30% para CPU, 70% para GPU

  // Início da execução
  inicio = omp_get_wtime(); 
  // Número de threads igual número de processadores + 1
  #pragma omp parallel num_threads(9)
  {
      // Thread master para comunicação e offloading para a GPU
      #pragma omp master 
      #pragma omp target data map(tofrom:soma_gpu) map(to:split_point) device(1)
      #pragma omp target teams distribute parallel for nowait reduction(+:soma_gpu)
      // Saltos de acordo com o número de processos (MPI)
      for (long int i = meu_ranque; i <= split_point; i += num_procs) { 
           double x = (i + 0.5) * passo;
           soma_gpu += 4.0 / (1.0 + x * x);
      }
      // Threads secundárias para execução na CPU
      #pragma omp for reduction(+:soma_cpu)
      for (long int i = split_point+(meu_ranque+1); i < num_passos; i += num_procs) { 
           double x = (i + 0.5) * passo;
           soma_cpu += 4.0 / (1.0 + x * x);
      }
  }
  // Calcula o valor local do processo
  soma_local = soma_gpu + soma_cpu;
  // MPI_Reduce soma os resultados parciais dos processos MPI
  MPI_Reduce(&soma_local, &soma_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  double pi = passo * soma_global; 
  // Processo raiz (0) finaliza e imprime os resultadr]os
  if (meu_ranque == 0) {     
      fim = omp_get_wtime();
      printf("Valor calculado de Pi: %3.15f em %lf segundos\n", pi, fim-inicio);
  } 
  // Finaliza o MPI
  MPI_Finalize();
  return 0;
}

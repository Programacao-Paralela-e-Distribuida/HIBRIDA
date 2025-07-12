#include <stdio.h>
#include <mpi.h>
const long num_passos = 10000000000;
int main(int argc, char *argv[]) {  /* mpi_calcpi.c */
  int meu_ranque, num_procs;
  double soma = 0.0, soma_global = 0.0;
  double inicio, fim;
  double passo = 1.0 / (double) num_passos;
  MPI_Init(&argc, &argv); // Inicia o MPI 
  MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  inicio = MPI_Wtime(); // Tempo de início da execução
  // Saltos de acordo com o número de processos
  for (long int i = meu_ranque; i < num_passos; i += num_procs) {
       double x = (i + 0.5) * passo;
       soma += 4.0 / (1.0 + x * x);
  }
  // MPI_Reduce soma os resultados parciais dos processos MPI
  MPI_Reduce(&soma, &soma_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  double pi = passo * soma_global;
  if (meu_ranque == 0) { // Processo 0 calcula o valor final de Pi
      fim = MPI_Wtime();
      printf("Valor calcula de Pi: %2.15f em %lf segundos\n", pi, fim-inicio);
  }
  MPI_Finalize(); // Finaliza o MPI
  return 0;
}

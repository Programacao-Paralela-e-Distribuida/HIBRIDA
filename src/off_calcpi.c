#include <stdio.h>
#include <omp.h>
#include <math.h>
int main() {  /* off_calcpi.c */
  long int num_passos = 10000000000; // Número de passos para a integração
  double passo = 1.0 / (double) num_passos;
  double pi = 0.0, inicio, fim;
  inicio = omp_get_wtime();
  #pragma omp target data map(tofrom: pi) map(to:num_passos, passo) device(1)// Diretiva para offloading para a GPU
  #pragma omp target teams distribute parallel for reduction(+:pi) // Paralelização com OpenMP
     for (long int i = 0; i < num_passos; i++) {
          double x = (i + 0.5) * passo;
          pi += 4.0 / (double) (1.0 + x * x); 
     }
  pi *= passo;
  fim = omp_get_wtime();
  printf("Valor de Pi calculado: %2.15f\n", pi);
  printf("Tempo de execução: %f segundos\n", fim - inicio);
  return 0;
}

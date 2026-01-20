#include <stdio.h>
#include <omp.h>
#include <math.h>
int main() {   /* omp_calcpi.c */
  static long num_passos = 10000000000;
  double passo = 1.0 / (double) num_passos;
  double soma = 0.0, pi = 0.0, inicio, fim;
  inicio = omp_get_wtime();
  #pragma omp parallel shared(soma, passo, num_passos) num_threads(8)
  #pragma omp for reduction(+:soma)
  for (long int i = 0; i < num_passos; i++) {
      double x = (i + 0.5) * passo;
      soma += 4.0 / (1.0 + x * x);
    }
  pi = soma * passo;  
  fim = omp_get_wtime();
  printf("Valor de Pi calculado: %3.15f. O tempo de execução foi %3.15f \n", pi, fim-inicio);
  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
// Defina a proporção da carga de trabalho para a GPU (em %)
static long int num_passos = 4000000000 ; // Número total de pontos aleatórios
int main(int argc, char *argv[]) {
    long int contagem=0;
    int ranque, numprocs, provided;
    double x, y, z, pi;
    double inicio, tempo;
    // Inicializa o MPI com suporte para threads
    // Inicializa o gerador de números aleatórios
    srand(time(NULL) + ranque);
    inicio = omp_get_wtime(); // Tempo de início da execução
    for (long int i = ranque; i < num_passos; i++) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        double z = x * x + y * y;
        if (z <= 1.0) contagem++;
    }
    pi = ((double)contagem / (double)num_passos) * 4.0; // Estimação de Pi
    tempo = omp_get_wtime() - inicio;
    printf("pi = %3.15f, %ld amostras, computados em %lf segundos\n", pi, num_passos, tempo);
    return 0;
}
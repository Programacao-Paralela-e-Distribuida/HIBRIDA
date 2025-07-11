#include <stdio.h>
#include "mpi.h"
#include <omp.h>
#include <stdlib.h>
#include <time.h>
// Defina a proporção da carga de trabalho para a GPU (em %)
#define GPU_WORKLOAD 70
static long int num_passos = 1000000000 ; // Número total de pontos aleatórios
int main(int argc, char *argv[]) {
    long int gpu_contagem_local = 0, cpu_contagem_local = 0, contagem_global = 0;
    int ranque, numprocs, provided;
    double x, y, z, pi;
    double inicio, tempo;
    // Inicializa o MPI com suporte para threads
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    if (provided < MPI_THREAD_FUNNELED) {
        printf("Nível de suporte para threads não é suficiente!\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &ranque); // O rank do processo
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs); // O número de processos
    // Inicializa o gerador de números aleatórios
    srand(time(NULL) + ranque);
    // Define a carga de trabalho para a GPU e para as threads CPU
    long int passos_gpu = num_passos * GPU_WORKLOAD / 100;
    long int passos_cpu = num_passos - passos_gpu;
    inicio = omp_get_wtime(); // Tempo de início da execução
    // Offloading com OpenMP para a GPU - Master thread
    #pragma omp parallel num_threads(5)
    {
        // Thread master para comunicação e offloading para a GPU
        #pragma omp master
        #pragma omp target data map(tofrom:gpu_contagem_local) map(alloc:x,y,z) device(1)
        #pragma omp target teams distribute parallel for reduction(+:gpu_contagem_local) nowait 
            for (long int i = ranque; i < passos_gpu; i += numprocs) {
                double x = (double)rand() / RAND_MAX;
                double y = (double)rand() / RAND_MAX;
                double z = x * x + y * y;
                if (z <= 1.0) gpu_contagem_local++; // Verifica se o ponto está no círculo
            }
            // Threads secundárias para execução no CPU
        #pragma omp for reduction(+:cpu_contagem_local)
            for (long int i = ranque; i < passos_cpu; i += numprocs) {
                double x = (double)rand() / RAND_MAX;
                double y = (double)rand() / RAND_MAX;
                double z = x * x + y * y;
                if (z <= 1.0) cpu_contagem_local++;
            }
    }
    // MPI_Reduce para somar os resultados de todos os processos MPI
    long int local_count_total = gpu_contagem_local + cpu_contagem_local;
    MPI_Reduce(&local_count_total, &contagem_global, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    if (ranque == 0) {
        pi = ((double)contagem_global / (double)num_passos) * 4.0; // Estimação de Pi
        tempo = omp_get_wtime() - inicio;
        printf("pi = %3.15f, %ld amostras, computados em %lf segundos\n", pi, num_passos, tempo);
    }
    MPI_Finalize(); // Finaliza o MPI
    return 0;
}

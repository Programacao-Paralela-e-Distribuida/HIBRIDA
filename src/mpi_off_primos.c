#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <mpi.h>

int is_prime(long int n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    for (long int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return 0;
    }
    return 1;
}
#define GPU_WORKLOAD 70

int main(int argc, char *argv[]) {
    int numprocs, rank, salto;
    long int num_primes_gpu = 0, num_primes_cpu = 0, global_num_primes = 0;
    long int  n, i, gpu_end;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (argc < 2) {
        printf("Valor inválido! Entre com o valor do maior inteiro\n");
       	return 0;
    } else {
        n = strtol(argv[1], (char **) NULL, 10);
    }

    gpu_end = n  * GPU_WORKLOAD / 100;
    if (gpu_end % 2 != 0) {
        gpu_end--; // Se for ímpar, subtrai 1 para tornar par
    }
    salto = numprocs*2;

    start_time = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp master
        {
            // Tarefa 1: Processamento na GPU
        #pragma omp task shared(num_primes_gpu, rank, n)
        {
        #pragma omp target data map(tofrom:num_primes_gpu) device(1)
        #pragma omp target teams distribute parallel for reduction(+:num_primes_gpu)
            for (long int i = 3+(rank*2); i <= gpu_end; i += salto) {
                if (is_prime(i)) num_primes_gpu++;
            }
        }
          // Tarefa 1: Processamento na CPU
        #pragma omp task shared(num_primes_cpu, rank, n)
        #pragma omp parallel for reduction(+:num_primes_cpu) num_threads(4)
            for (long int j = gpu_end+(rank*2)+1; j <= n; j += salto) {
                if (is_prime(j)) num_primes_cpu++;
            }
        }
    }
    long int local_num_primes = num_primes_gpu + num_primes_cpu;
    printf("Total local %d  %ld\n", rank, local_num_primes);
    MPI_Reduce(&local_num_primes, &global_num_primes, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    global_num_primes++; // Adiciona 2, que é o único número par primo
    if (rank == 0) {
        end_time = omp_get_wtime();
        printf("Total de primos até %ld: %ld\n", n, global_num_primes);
        printf("Calculado em %lf segundos\n", end_time - start_time);
    }
    MPI_Finalize();
    return 0;
}
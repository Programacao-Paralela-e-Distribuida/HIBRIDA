#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int is_prime(long int n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    for (long int i = 5; i * i <= n; i += 6) {
         if (n % i == 0 || n % (i + 2) == 0) return 0;
    }
    return 1;
}
#define GPU_WORKLOAD 70 // Percentual de trabalho para a GPU
int main(int argc, char *argv[]) {
    long int num_primes_gpu = 0, num_primes_cpu = 0, num_primes=0;
    long int  n, gpu_end;
    double start_time, end_time;

    if (argc < 2) {
        n = 100000000; // Valor padrão se nenhum argumento for fornecido
    } else {
        n = strtol(argv[1], (char **) NULL, 10);
    }

    gpu_end = n  * GPU_WORKLOAD / 100;
    if (gpu_end % 2 != 0) {
        gpu_end--; // Se for ímpar, subtrai 1 para tornar par
    }
    start_time = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp master
        {
        // Tarefa 1: Processamento na GPU
        #pragma omp task shared(num_primes_gpu, n)
        {
        #pragma omp target data map(tofrom:num_primes_gpu) device(1)
        #pragma omp target teams distribute parallel for reduction(+:num_primes_gpu)
            for (long int i = 3; i <= gpu_end; i++) {
                if (is_prime(i)) num_primes_gpu++;
            }
        }
        // Tarefa 2: Processamento na CPU
        #pragma omp task shared(num_primes_cpu, n)
        #pragma omp parallel for reduction(+:num_primes_cpu) num_threads(4)
            for (long int j = gpu_end+1; j <= n; j++) {
                if (is_prime(j)) num_primes_cpu++;
            }
        }
    } // Fim da região paralela - Barreira implícita aqui
    num_primes = num_primes_gpu + num_primes_cpu+1; // Adiciona 2, que é o único número par primo
    end_time = omp_get_wtime();
    printf("Total de primos até %ld: %ld\n", n, num_primes);
    printf("Calculado em %lf segundos\n", end_time - start_time);
    return 0;
}
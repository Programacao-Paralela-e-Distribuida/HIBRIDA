#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

// Função para verificar se um número é primo
bool is_prime(int num) {
    if (num <= 1) return false;
    if (num == 2) return true;
    if (num % 2 == 0) return false;

    for (int i = 3; i <= sqrt(num); i += 2) {
        if (num % i == 0) return false;
    }
    return true;
}

int main(int argc, char *argv[]) {
    int N;
        if (argc < 2) {
        printf("Valor inválido! Entre com o valor do maior inteiro\n");
       	return 0;
    } else {
        N = strtol(argv[1], (char **) NULL, 10);
    }

    int total_primes_cpu = 0;
    int total_primes_gpu = 0;

    // Dividir o trabalho entre CPU e GPU
    int split_point = N * .3; // 30% para CPU, 70% para GPU
    double inicio = omp_get_wtime();
    // Criar duas tarefas assíncronas
    #pragma omp parallel
    {
        #pragma omp master
        {
            // Tarefa 1: Processamento na CPU
            #pragma omp task shared(total_primes_cpu, split_point)
            {
                #pragma omp parallel for reduction(+:total_primes_cpu) num_threads(16)
                for (int i = 1; i <= split_point; i++) {
                    if (is_prime(i)) {
                        total_primes_cpu++;
                    }
                }
            }

            // Tarefa 2: Processamento na GPU
            #pragma omp task shared(total_primes_gpu, split_point)
            {
                #pragma omp target teams distribute parallel for reduction(+:total_primes_gpu) map(tofrom: total_primes_gpu)
                for (int i = split_point + 1; i <= N; i++) {
                    if (is_prime(i)) {
                        total_primes_gpu++;
                    }
                }
            }

            // Aguardar a conclusão das duas tarefas
            #pragma omp taskwait
        }
    }

    // Soma os resultados da CPU e GPU
    int total_primes = total_primes_cpu + total_primes_gpu;
    double tempo = omp_get_wtime() - inicio;
    printf("Total de números primos entre 1 e %d: %d. Calculado em %f segundos.\n", N, total_primes, tempo);
    printf("Primos encontrados na CPU: %d\n", total_primes_cpu);
    printf("Primos encontrados na GPU: %d\n", total_primes_gpu);

    return 0;
}

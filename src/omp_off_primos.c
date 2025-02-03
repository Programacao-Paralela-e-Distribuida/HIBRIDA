#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

// Função para verificar se um número é primo
bool is_prime2(int num) {
    for (int i = 3; i <= sqrt(num); i += 2) {
        if (num % i == 0) return false;
    }
    return true;
}

bool is_prime(long int n) {
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (long int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
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
    int split_point = (int)(N * .3); // 30% para CPU, 70% para GPU
    if (split_point % 2 != 0) 
        split_point--; // Se for ímpar, subtrai 1 para tornar par
    double inicio = omp_get_wtime();
    // Criar duas tarefas assíncronas
    #pragma omp parallel
    {
        #pragma omp master
        {
            // Tarefa 1: Processamento na CPU
            #pragma omp task shared(total_primes_cpu, split_point)
            #pragma omp parallel for reduction(+:total_primes_cpu) num_threads(16)
            for (int i = 3; i <= split_point; i +=2) {
                 if (is_prime(i)) 
                    total_primes_cpu++;
            }

            // Tarefa 2: Processamento na GPU
            #pragma omp task shared(total_primes_gpu, split_point)
            #pragma omp target teams distribute parallel for reduction(+:total_primes_gpu) map(tofrom: total_primes_gpu)
            for (int i = split_point + 1; i <= N; i+=2) {
                if (is_prime(i)) 
                    total_primes_gpu++;
            }
            // Aguardar a conclusão das duas tarefas
            #pragma omp taskwait
        }
    }

    // Soma os resultados da CPU e GPU
    int total_primes = total_primes_cpu + total_primes_gpu;
    total_primes++; // Adiciona o número 2, que não foi verificado
    double tempo = omp_get_wtime() - inicio;
    printf("Total de números primos entre 1 e %d: %d. Calculado em %f segundos.\n", N, total_primes, tempo);
    printf("Primos encontrados na CPU: %d\n", total_primes_cpu);
    printf("Primos encontrados na GPU: %d\n", total_primes_gpu);
    printf("Ponto de Divisão: %d\n", split_point);
    return 0;
}

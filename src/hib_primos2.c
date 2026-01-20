#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>
// Função para verificar se um número é primo
bool is_prime(long int n) {
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (long int i = 5; i * i <= n; i += 6) 
        if (n % i == 0 || n % (i + 2) == 0) return false;
    return true;
}
int main(int argc, char *argv[]) { /* hib_primos2.c */
int N, total_primes;
int fornecido, ranque, num_procs;
    if (argc < 2) {
        printf("Valor inválido! Entre com o valor do maior inteiro\n");
       	return 0;
    } else {
        N = strtol(argv[1], (char **) NULL, 10);
    }

    int local_primes_cpu = 0;
    int local_primes_gpu = 0;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &fornecido);
    MPI_Comm_rank(MPI_COMM_WORLD, &ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
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
            #pragma omp task shared(local_primes_cpu, split_point)
            #pragma omp parallel for reduction(+:local_primes_cpu) num_threads(16)
            for (int i = 3+(ranque*2); i <= split_point; i +=2*num_procs) {
                 if (is_prime(i)) 
                    local_primes_cpu++;
            }
            // Tarefa 2: Processamento na GPU
            #pragma omp task shared(local_primes_gpu, split_point)
            #pragma omp target teams distribute parallel for reduction(+:local_primes_gpu) map(tofrom: local_primes_gpu)
            for (int i = split_point + 1 + ranque*2; i <= N; i+=2*num_procs) {
                if (is_prime(i)) 
                    local_primes_gpu++;
            }
            // Aguardar a conclusão das duas tarefas
            #pragma omp taskwait
        }
    }

    // Soma os resultados da CPU e GPU
    int local_primes = local_primes_cpu + local_primes_gpu;
    MPI_Reduce(&local_primes, &total_primes, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (ranque == 0) {
    total_primes++; // Adiciona o número 2, que não foi verificado
    double tempo = omp_get_wtime() - inicio;
    printf("Total de números primos entre 1 e %d: %d. Calculado em %f segundos.\n", N, total_primes, tempo);
    printf("Primos encontrados na CPU: %d\n", local_primes_cpu);
    printf("Primos encontrados na GPU: %d\n", local_primes_gpu);
    printf("Ponto de Divisão: %d\n", split_point);
    }
    MPI_Finalize();
    return 0;
}

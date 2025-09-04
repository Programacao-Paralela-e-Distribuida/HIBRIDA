#include <stdio.h>
#include <time.h>
static long num_passos = 10000000000;
double passo;
int main(int argc, char *argv[]) {
    double x, pi, soma = 0.0;
    clock_t inicio, fim;
    passo = 1.0 / (double)num_passos;
    inicio = clock(); // Tempo de início da execução
    for (long int i = 0; i < num_passos; i++) {
        x = (i + 0.5) * passo;
        soma += 4.0 / (1.0 + x * x);
    }
    pi = passo * soma;
    fim = clock(); // Tempo final
    double tempo = ((double)(fim - inicio)) / CLOCKS_PER_SEC; // Tempo em segundos
    printf("pi = %2.15f, %ld passos, computados em %lf segundos\n", pi, num_passos, tempo);
    return 0;
}

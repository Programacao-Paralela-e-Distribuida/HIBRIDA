#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
// Função para verificar se um número é primo
bool is_prime(long int n) {
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (long int i = 5; i * i <= n; i += 6) 
        if (n % i == 0 || n % (i + 2) == 0) return false;
    return true;
}
int main(int argc, char *argv[]) {
long int N, total_primes=0;
    if (argc < 2) {
        printf("Valor inválido! Entre com o valor do maior inteiro\n");
        return 0;
    } else {
        N = strtol(argv[1], (char **) NULL, 10);
    }
    // Calcula só para os números ímpares, começando com 3
    for (int i = 3; i <= N ; i+=2) 
         if (is_prime(i)) total_primes++;
    total_primes++;  // O número 2 também é primo
    // Imprime os resultados
    printf("Total de números primos entre 1 e %ld: %ld\n", N, total_primes);
    return 0;
}
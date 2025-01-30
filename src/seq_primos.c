#include <stdio.h>
#include <stdbool.h>
#include <math.h>

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

int main() {
    long int N, total_primes=0;
    printf("Digite o valor de N: ");
    scanf("%ld", &N);

    // Dividir o trabalho entre CPU e GPU
    for (int i = 1; i <= N ; i+=2) {
         if (is_prime(i)) {
             total_primes++;
         }
    }
    total_primes++;  // O número 2 também é primo

    // Soma os resultados da CPU e GPU

    printf("Total de números primos entre 1 e %ld: %ld\n", N, total_primes);
    return 0;
}

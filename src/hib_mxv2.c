#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <omp.h>

/* Função de cálculo no Acelerador (GPU) */
void mxv(long int m_local, long int n, double* A, double* b, double* c)
{
    long int i, j;
    double temp;

    /* * map(to: A[...]): Mapeia apenas as linhas locais deste nó para a GPU.
     * teams distribute parallel for: Paraleliza o loop externo entre os núcleos da GPU.
     */
    #pragma omp target teams distribute parallel for \
        map(to: A[0:m_local*n], b[0:n]) \
        map(from: c[0:m_local]) \
        private(j, temp)
    for (i = 0; i < m_local; i++) {
        temp = 0.0;
        for (j = 0; j < n; j++) {
            temp += A[i*n + j] * b[j];
        }
        c[i] = temp;
    }
}

int main(int argc, char *argv[]) {
    double *Aloc, *b, *cloc, *c;
    long int i, j, m, n, m_local;
    int meu_ranque, num_procs, raiz=0;
    double inicio, fim;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    /* * MUDANÇA: Leitura dos parâmetros
     * argv[1] agora é 'm_local' (linhas por processo), não o total.
     */
    if (argc < 3) {
        if (meu_ranque == 0)
            printf("Uso: %s <linhas_por_processo> <colunas_n>\n", argv[0]);
        MPI_Finalize();
        return 0;
    } else {
        m_local = atol(argv[1]); /* Linhas a serem processadas por ESTE nó */
        n = atol(argv[2]);       /* Colunas (dimensão comum) */
    }

    /* O tamanho total da matriz global 'm' é derivado aqui */
    m = m_local * num_procs;

    if (meu_ranque == 0){
        printf("Configuração:\n");
        printf(" - Processos MPI: %d\n", num_procs);
        printf(" - Linhas por nó (m_local): %ld\n", m_local);
        printf(" - Colunas (n): %ld\n", n);
        printf(" - Dimensão Global da Matriz: %ld x %ld\n", m, n);
    }

    /* Alocações baseadas no tamanho local */
    Aloc = (double *) malloc(m_local * n * sizeof(double));
    b    = (double *) malloc(n * sizeof(double));
    cloc = (double *) malloc(m_local * sizeof(double));
    
    /* Vetor global 'c' para reunir resultados (apenas na raiz ou em todos se necessário) */
    c    = (double *) malloc(m * sizeof(double));

    /* Inicialização dos dados */
    for (j = 0; j < n; j++)
        b[j] = 2.0;
    
    /* Preenchimento da matriz local */
    for (i = 0; i < m_local; i++) {
        for (j = 0; j < n; j++) {
            /* * O cálculo do valor precisa considerar o deslocamento global 
             * se quisermos manter a lógica matemática original.
             * Linha global = (meu_ranque * m_local) + i
             */
            long int linha_global = (meu_ranque * m_local) + i;
            Aloc[n*i + j] = (double) (linha_global + j); 
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    inicio = MPI_Wtime();

    /* Chamada ao Kernel Offload (GPU) */
    mxv(m_local, n, Aloc, b, cloc);

    /* Coleta os resultados parciais */
    MPI_Allgather(cloc, m_local, MPI_DOUBLE, c, m_local, MPI_DOUBLE, MPI_COMM_WORLD);

    fim = MPI_Wtime();

    if (meu_ranque == raiz) {
        printf("Tempo total = %f segundos\n", fim - inicio);
    }

    free(Aloc);
    free(b);
    free(c);
    free(cloc);

    MPI_Finalize();
    return(0);
}

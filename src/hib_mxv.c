#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <omp.h>
void mxv(long int m, long int n, double* A, double* b, double* c)
{
long int i, j;
register double temp;
   #pragma omp parallel for private(j, temp) num_threads(4)
   for (i = 0; i < m; i++) {
       temp = 0.0;
       for (j = 0; j < n; j++)
            temp += A[i*n + j]*b[j];
       c[i] = temp;
   }
}
int main(int argc, char *argv[]) { /* hib_mxv.c  */
double *Aloc, *b,*cloc, *c;
/* matriz m x n por um vetor de dimensão n */
long int i, j, m, n;
int meu_ranque, num_procs, raiz=0;
double inicio, fim;
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
   MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

   /* Os processos faemz a leitura da dimensão m e n  como parametros */
   if (argc < 3) {
        if (meu_ranque == 0)
        printf("Entre com as dimensões da matriz: m e n valores inteiros, sendo m múltiplo do número de processos \n");
        return 0;
    } else {
        m = atoi(argv[1]);
        n = atoi(argv[2]);
    }
    if (meu_ranque == 0){
        printf("Valor de m: %ld e n: %ld \n", m,n);
    }
    Aloc=(double *) malloc(m/num_procs*n*sizeof(double));
    b=(double*) malloc(n*sizeof(double));
    c=(double*) malloc(m*sizeof(double));
    cloc=(double*) malloc(m/num_procs*sizeof(double));

    /* Atribuindo valor inicial à matriz A e ao vetor b */
     for (j = 0; j < n; j++)
              b[j] = 2.0;
     for (i = 0; i < m/num_procs; i++)
          for (j = 0; j < n; j++)
              Aloc[n*i+j] = (double) j;
     inicio = MPI_Wtime();
    /* Cada processo faz o cálculo parcial de 'c'  */
     mxv(m/num_procs, n, Aloc, b, cloc);
     MPI_Barrier(MPI_COMM_WORLD);
    /* O processo raiz coleta o vetor 'c' dos demais processos */
     MPI_Allgather(cloc, m/num_procs, MPI_DOUBLE, c, m/num_procs, MPI_DOUBLE, MPI_COMM_WORLD);
     fim = MPI_Wtime();
    /* O processo raiz imprime o resultado */
     if (meu_ranque == raiz) {
        printf("Tempo total = %f\n", fim-inicio);
     }
     free(Aloc);
     free(b);
     free(c);
     free(cloc);

     MPI_Finalize();
     return(0);
}

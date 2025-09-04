#include <stdio.h>
#include <mpi.h>
int main(int argc, char *argv[])  /* mpi_level.c */
{
  int fornecido;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE,&fornecido);
  switch (fornecido) { 
	case MPI_THREAD_SINGLE:
      		printf("Executando com MPI_THREAD_SINGLE\n");
		break;
	case MPI_THREAD_FUNNELED:
      		printf("Executando com MPI_THREAD_FUNNELED\n");
      		break;
	case MPI_THREAD_SERIALIZED:
		printf("Executando com MPI_THREAD_SERIALIZED\n");
		break;
	case MPI_THREAD_MULTIPLE:
      		printf("Executando com MPI_THREAD_MULTIPLE\n");
		break;
  }
  MPI_Finalize();
}

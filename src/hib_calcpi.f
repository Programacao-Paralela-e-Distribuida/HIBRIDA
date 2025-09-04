
program mpi_omp_calc_pi
  use mpi
  implicit none
  integer :: num_steps, rank, numprocs, provided, ierr
  double precision :: step, x, pi, local_sum, global_sum
  double precision :: start_time, elapsed_time
  integer :: i

  num_steps = 10000000000
  step = 1.0d0 / num_steps
  local_sum = 0.0d0
  global_sum = 0.0d0

  call MPI_Init_thread(MPI_THREAD_FUNNELED, provided, ierr)
  if (provided < MPI_THREAD_FUNNELED) then
     print *, "Nível de suporte para threads não é suficiente!"
     call MPI_Abort(MPI_COMM_WORLD, 1, ierr)
  end if

  call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
  call MPI_Comm_size(MPI_COMM_WORLD, numprocs, ierr)

  start_time = omp_get_wtime()

!$omp parallel do private(x) shared(rank, num_steps, numprocs, step) reduction(+:local_sum) num_threads(4)
  do i = rank, num_steps - 1, numprocs
     x = (i + 0.5d0) * step
     local_sum = local_sum + 4.0d0 / (1.0d0 + x * x)
  end do
!$omp end parallel do

  print *, "Processo ", rank, " Redução ", local_sum
  call MPI_Reduce(local_sum, global_sum, 1, MPI_DOUBLE_PRECISION, MPI_SUM, 0, MPI_COMM_WORLD, ierr)

  if (rank == 0) then
     pi = step * global_sum
     elapsed_time = omp_get_wtime() - start_time
     print *, "pi = ", pi, ", ", num_steps, " passos, computados em ", elapsed_time, " segundos"
  end if

  call MPI_Finalize(ierr)
end program mpi_omp_calc_pi


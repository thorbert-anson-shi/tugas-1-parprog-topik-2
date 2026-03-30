#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int world_rank;

  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // ID proses

  printf("Hello world from process %d\n", world_rank);

  MPI_Finalize();
  return 0;
}

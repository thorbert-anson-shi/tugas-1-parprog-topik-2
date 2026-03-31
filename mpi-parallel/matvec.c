#include <mpi.h>
#include <stdlib.h>

void matvec(double *A, double *x, double *y, int n, int m) {
  int rank, n_nodes;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_nodes);

  int sendcounts[n_nodes];
  int displs[n_nodes];
  int recvcounts[n_nodes];
  int recv_displs[n_nodes];

  int current_a_offset = 0;
  int current_y_offset = 0;

  for (int i = 0; i < n_nodes; i++) {
    int rows_for_rank = (n / n_nodes) + (i < (n % n_nodes) ? 1 : 0);

    sendcounts[i] = rows_for_rank * m;
    displs[i] = current_a_offset;
    current_a_offset += sendcounts[i];

    recvcounts[i] = rows_for_rank;
    recv_displs[i] = current_y_offset;
    current_y_offset += recvcounts[i];
  }

  int local_n = (n / n_nodes) + (rank < (n % n_nodes) ? 1 : 0);
  double *local_A = malloc(local_n * m * sizeof(double));
  double *local_y = malloc(local_n * sizeof(double));

  // Scatter rows of A to each rank
  MPI_Scatterv(A, sendcounts, displs, MPI_DOUBLE, local_A, local_n * m,
               MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Broadcast x to all nodes
  MPI_Bcast(x, m, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  for (int i = 0; i < local_n; i++) {
    double sum = 0.0;
    for (int j = 0; j < m; j++) {
      sum += local_A[i * m + j] * x[j];
    }
    local_y[i] = sum;
  }

  // Gather the partial results of y back to the root node
  MPI_Gatherv(local_y, local_n, MPI_DOUBLE, y, recvcounts, recv_displs,
              MPI_DOUBLE, 0, MPI_COMM_WORLD);

  free(local_A);
  free(local_y);
}

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A simple flat 2D matrix (row-major) for easy MPI communication
typedef struct {
  double *data;
  int rows;
  int cols;
} Matrix;

Matrix *alloc_matrix(int rows, int cols) {
  Matrix *mat = malloc(sizeof(Matrix));
  mat->rows = rows;
  mat->cols = cols;
  mat->data = calloc(rows * cols, sizeof(double));
  return mat;
}

void free_matrix(Matrix *mat) {
  if (!mat)
    return;
  free(mat->data);
  free(mat);
}

// Indexing macro for flat row-major storage
#define MAT(m, i, j) ((m)->data[(i) * (m)->cols + (j)])

void print_matrix(Matrix *mat) {
  printf("[");
  for (int i = 0; i < mat->rows; i++) {
    printf("[");
    for (int j = 0; j < mat->cols; j++) {
      printf("%.1f", MAT(mat, i, j));
      if (j < mat->cols - 1)
        printf(", ");
    }
    printf("]");
    if (i < mat->rows - 1)
      printf(", ");
  }
  printf("]\n");
}

// Each process computes its assigned rows of the result
Matrix *matmul_mpi(Matrix *a, Matrix *b, int n) {
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Divide rows of A among processes
  int rows_per_proc = n / size;
  int leftover = n % size;

  // sendcounts and displacements for MPI_Scatterv
  int *sendcounts = malloc(size * sizeof(int));
  int *displs = malloc(size * sizeof(int));
  int offset = 0;
  for (int i = 0; i < size; i++) {
    // distribute leftover rows to the first `leftover` processes
    sendcounts[i] = (rows_per_proc + (i < leftover ? 1 : 0)) * n;
    displs[i] = offset;
    offset += sendcounts[i];
  }

  int local_rows = rows_per_proc + (rank < leftover ? 1 : 0);

  // Local slice of A
  double *local_a = malloc(local_rows * n * sizeof(double));

  // Scatter rows of A to all processes
  MPI_Scatterv(rank == 0 ? a->data : NULL, sendcounts, displs, MPI_DOUBLE,
               local_a, local_rows * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Broadcast full B to all processes
  double *b_data = malloc(n * n * sizeof(double));
  if (rank == 0)
    memcpy(b_data, b->data, n * n * sizeof(double));
  MPI_Bcast(b_data, n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Each process computes its local result rows
  double *local_result = calloc(local_rows * n, sizeof(double));
  for (int i = 0; i < local_rows; i++) {
    for (int j = 0; j < n; j++) {
      double sum = 0.0;
      for (int k = 0; k < n; k++) {
        sum += local_a[i * n + k] * b_data[k * n + j];
      }
      local_result[i * n + j] = sum;
    }
  }

  // Gather results back to root
  Matrix *result = NULL;
  if (rank == 0)
    result = alloc_matrix(n, n);

  MPI_Gatherv(local_result, local_rows * n, MPI_DOUBLE,
              rank == 0 ? result->data : NULL, sendcounts, displs, MPI_DOUBLE,
              0, MPI_COMM_WORLD);

  free(local_a);
  free(b_data);
  free(local_result);
  free(sendcounts);
  free(displs);

  return rank == 0 ? result : NULL;
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int sizes[4] = {512, 1024, 2048, 4096};

  for (int s = 0; s < 4; s++) {
    int n = sizes[s];

    Matrix *a = NULL, *b = NULL;
    if (rank == 0) {
      a = alloc_matrix(n, n);
      b = alloc_matrix(n, n);
      for (int i = 0; i < n * n; i++) {
        a->data[i] = (double)(rand() % 20);
        b->data[i] = (double)(rand() % 20);
      }
    }

    if (rank == 0) {
      printf("Running %dx%d matmul...\n", n, n);
      fflush(stdout);
    }

    double start = MPI_Wtime();
    Matrix *result = matmul_mpi(a, b, n);
    double elapsed = MPI_Wtime() - start;

    if (rank == 0) {
      printf("Time taken: %.6f seconds\n", elapsed);
      fflush(stdout);
      free_matrix(result);
      free_matrix(a);
      free_matrix(b);
    }
  }

  MPI_Finalize();
  return 0;
}

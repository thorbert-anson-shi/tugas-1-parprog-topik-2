#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
  double **data;
  int rows;
  int cols;
} Matrix;

Matrix *alloc_matrix(int rows, int cols) {
  Matrix *mat = malloc(sizeof(Matrix));
  mat->rows = rows;
  mat->cols = cols;
  mat->data = malloc(rows * sizeof(double *));
  for (int i = 0; i < rows; i++) {
    mat->data[i] = calloc(cols, sizeof(double));
  }
  return mat;
}

void free_matrix(Matrix *mat) {
  if (!mat)
    return;
  for (int i = 0; i < mat->rows; i++) {
    free(mat->data[i]);
  }
  free(mat->data);
  free(mat);
}

void print_matrix(Matrix *mat) {
  printf("[");
  for (int i = 0; i < mat->rows; i++) {
    printf("[");
    for (int j = 0; j < mat->cols; j++) {
      printf("%.1f", mat->data[i][j]);
      if (j < mat->cols - 1)
        printf(", ");
    }
    printf("]");
    if (i < mat->rows - 1)
      printf(", ");
  }
  printf("]\n");
}

Matrix *matmul(Matrix *a, Matrix *b) {
  if (a->cols != b->rows) {
    printf("incompatible matrix dimensions: (%d x %d) x (%d x %d)\n", a->rows,
           a->cols, b->rows, b->cols);
    return NULL;
  }

  Matrix *result = alloc_matrix(a->rows, b->cols);

#pragma omp parallel for collapse(2)
  for (int i = 0; i < a->rows; i++) {
    for (int j = 0; j < b->cols; j++) {
      double sum = 0.0;
      for (int k = 0; k < a->cols; k++) {
        sum += a->data[i][k] * b->data[k][j];
      }
      result->data[i][j] = sum;
    }
  }

  return result;
}

int randrange(int min, int max) { return rand() % max + min; }

int main() {
  int n[4] = {512, 1024, 2048, 4096};

  for (int i = 0; i < 4; i++) {
    int num_dims = n[i];

    printf("Num dims: %d", num_dims);

    Matrix *a = alloc_matrix(num_dims, num_dims);
    for (int row = 0; row < num_dims; row++) {
      for (int col = 0; col < num_dims; col++) {
        a->data[row][col] = randrange(0, 20);
      }
    }

    Matrix *b = alloc_matrix(num_dims, num_dims);
    for (int row = 0; row < num_dims; row++) {
      for (int col = 0; col < num_dims; col++) {
        b->data[row][col] = randrange(0, 20);
      }
    }

    double start, elapsed;

    start = omp_get_wtime();

    Matrix *result = matmul(a, b);
    if (result) {
      elapsed = omp_get_wtime() - start;

      printf("Time taken: %.6f seconds\n", elapsed);
      free_matrix(result);
    }

    free_matrix(a);
    free_matrix(b);
  }

  return 0;
}

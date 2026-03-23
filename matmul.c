#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  double **data;
  int rows;
  int cols;
} Matrix;

// Allocate a matrix of given size
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

// Free a matrix from memory
void free_matrix(Matrix *mat) {
  if (!mat)
    return;
  for (int i = 0; i < mat->rows; i++) {
    free(mat->data[i]);
  }
  free(mat->data);
  free(mat);
}

// Print a matrix
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

// Multiply a square matrix by a vector (returns new vector as Matrix with 1
// col) vec is passed as a flat double array of length n
double *mat_vec_mul(Matrix *mat, double *vec, int vec_len) {
  if (mat->rows != vec_len)
    return NULL;
  // Check square matrix
  if (mat->rows != mat->cols)
    return NULL;

  int n = vec_len;
  double *ans = calloc(n, sizeof(double));
  for (int component = 0; component < n; component++) {
    for (int dim = 0; dim < n; dim++) {
      ans[component] += vec[component] * mat->data[dim][component];
    }
  }
  return ans;
}

// Transpose a square matrix
Matrix *mat_transpose(Matrix *mat) {
  if (mat->rows != mat->cols)
    return NULL;
  int n = mat->rows;
  Matrix *ans = alloc_matrix(n, n);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      ans->data[j][i] = mat->data[i][j];
    }
  }
  return ans;
}

// Multiply two matrices A x B
Matrix *matmul(Matrix *a, Matrix *b) {
  Matrix *b_transpose = mat_transpose(b);
  if (!b_transpose) {
    printf("matrix B needs to be a square matrix\n");
    return NULL;
  }

  int n = b_transpose->rows;
  Matrix *ans_transposed = alloc_matrix(n, n);

  for (int i = 0; i < b_transpose->rows; i++) {
    double *ans_row = mat_vec_mul(a, b_transpose->data[i], b_transpose->cols);
    if (!ans_row) {
      printf("the matrices are of incompatible dimensionality\n");
      free_matrix(b_transpose);
      free_matrix(ans_transposed);
      return NULL;
    }
    memcpy(ans_transposed->data[i], ans_row, n * sizeof(double));
    free(ans_row);
  }

  Matrix *result = mat_transpose(ans_transposed);
  free_matrix(b_transpose);
  free_matrix(ans_transposed);
  return result;
}

int main() {
  Matrix *a = alloc_matrix(2, 2);
  a->data[0][0] = 1.0;
  a->data[0][1] = 0.0;
  a->data[1][0] = 0.0;
  a->data[1][1] = 2.0;

  Matrix *b = alloc_matrix(2, 2);
  b->data[0][0] = 2.0;
  b->data[0][1] = 3.0;
  b->data[1][0] = 5.0;
  b->data[1][1] = 4.0;

  Matrix *result = matmul(a, b);
  if (result) {
    print_matrix(result);
    free_matrix(result);
  }

  free_matrix(a);
  free_matrix(b);
  return 0;
}

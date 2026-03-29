#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

void matvec_omp(double *A, double *x, double *y, int n, int m) {
#pragma omp parallel for schedule(static)
  for (int i = 0; i < n; i++) {
    double sum = 0.0;

#pragma omp simd reduction(+ : sum)
    for (int j = 0; j < m; j++) {
      sum += A[i * m + j] * x[j];
    }

    y[i] = sum;
  }
}

int main() {
  int n = 4096, m = 4096;

  double *A = malloc(n * m * sizeof(double));
  double *x = malloc(m * sizeof(double));
  double *y = malloc(n * sizeof(double));

  for (int i = 0; i < n * m; i++)
    A[i] = 1.0;
  for (int i = 0; i < m; i++)
    x[i] = 1.0;

  double t1 = omp_get_wtime();
  matvec_omp(A, x, y, n, m);
  double t2 = omp_get_wtime();

  printf("%f\n", t2 - t1);

  free(A);
  free(x);
  free(y);
  return 0;
}

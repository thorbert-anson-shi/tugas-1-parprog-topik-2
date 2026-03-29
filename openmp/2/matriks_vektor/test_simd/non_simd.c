#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  int n = 10000000;
  double *a = malloc(n * sizeof(double));
  double *b = malloc(n * sizeof(double));
  double sum = 0.0;

  for (int i = 0; i < n; i++) {
    a[i] = 1.1;
    b[i] = 2.2;
  }

  double t1 = omp_get_wtime();

  for (int i = 0; i < n; i++) {
    double v = a[i] * b[i];

    // bikin compute berat
    v = v * v;
    v = v * v;
    v = v * v;
    v = v * v;

    sum += v;
  }

  double t2 = omp_get_wtime();

  printf("NON SIMD: %f\n", t2 - t1, sum);

  free(a);
  free(b);
}

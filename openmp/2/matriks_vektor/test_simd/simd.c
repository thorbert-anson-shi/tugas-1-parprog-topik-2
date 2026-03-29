#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  int n = 10000000;
  double *a = aligned_alloc(32, n * sizeof(double));
  double *b = aligned_alloc(32, n * sizeof(double));
  double sum = 0.0;

  for (int i = 0; i < n; i++) {
    a[i] = 1.1;
    b[i] = 2.2;
  }

  double t1 = omp_get_wtime();

#pragma omp simd reduction(+ : sum)
  for (int i = 0; i < n; i++) {
    double v = a[i] * b[i];

    // random-ish math → gak bisa dioptimasi gampang
    v = v * 1.000001 + 0.000001;
    v = v * v + 1.0;
    v = v / (v + 1.0);
    v = v * v * v;
    sum += v;
  }

  double t2 = omp_get_wtime();

  printf("SIMD: %f (sum=%f)\n", t2 - t1, sum);

  free(a);
  free(b);
}

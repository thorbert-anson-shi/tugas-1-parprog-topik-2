#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

// lebih baik contigus di operasi READ Sr. ChatGPT
void transpose(double *B, double *B_T, int N) {
#pragma omp parallel for schedule(static)
  for (int j = 0; j < N; j++) {
#pragma omp simd
    for (int i = 0; i < N; i++) {
      B_T[j * N + i] = B[i * N + j];
    }
  }
}

void matmul_avx2(double *A, double *B, double *C, int N) {
#pragma omp parallel for schedule(static)
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      double sum = 0.0;
#pragma omp simd reduction(+ : sum)
      for (int k = 0; k < N; k++) {
        sum += A[i * N + k] * B[j * N + k];
      }
      C[i * N + j] = sum;
    }
  }
}

int main() {
  int N = 4096;
  double *A = aligned_alloc(32, N * N * sizeof(double));
  double *B = aligned_alloc(32, N * N * sizeof(double));
  double *B_T = aligned_alloc(32, N * N * sizeof(double));
  double *C = aligned_alloc(32, N * N * sizeof(double));

  for (int i = 0; i < N * N; i++)
    A[i] = 2.0;
  for (int i = 0; i < N * N; i++)
    B[i] = 2.0;
  for (int i = 0; i < N * N; i++)
    C[i] = 0.0;

  int repeat = 1;
  double total_time = 0.0;

  for (int r = 0; r < repeat; r++) {
    double t1 = omp_get_wtime();
    transpose(B, B_T, N);
    matmul_avx2(A, B_T, C, N);
    double t2 = omp_get_wtime();
    total_time += (t2 - t1);
  }

  printf("Average time over %d runs: %f s\n", repeat, total_time / repeat);
  printf("C[0][0] = %f\n", C[0]);

  free(A);
  free(B);
  free(B_T);
  free(C);
  return 0;
}

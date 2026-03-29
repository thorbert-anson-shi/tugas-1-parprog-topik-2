gcc -O3 program.c
full optimizationis

gcc -O3 -fopenmp -fno-tree-vectorize non_simd.c -o non_simd
gcc -O3 -fopenmp simd.c -o simd

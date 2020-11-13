#include <iostream>
#include <cstdlib>
#include <time.h>
#include <omp.h>
#include <stdlib.h>
#include <assert.h>

#include "util.h"


template <typename T> 
__device__ void compare_and_swap(T * a, T * b, int inc) {
  if ((*a > *b && inc) || (*a < *b && !inc)) {
    T tmp = *a;
    *a = *b;
    *b = tmp;
  }
}

template <typename T> 
__global__ void bitonic_merge(T * arr, int split_size, int bm_size) {

  int idx = blockDim.x * blockIdx.x + threadIdx.x;
  int cmp_idx = idx ^ split_size;
  if (idx < cmp_idx) {
    compare_and_swap<T>(&arr[idx], &arr[cmp_idx], (idx & bm_size) == 0);
  }

}

int is_power_of_2(int x) {
  return x > 0 && (((x-1) & x) == 0);
}

template <typename T> 
void cuda_main(int size) {
  int num_bytes = size * sizeof(T);
  
  // change
  T * arr = gen_data_real<T>(size, 0, std::numeric_limits<T>::max());
  
  T * d_arr;

  cudaMalloc((void **)&d_arr, num_bytes);

  int num_blocks = size / 512;
  int num_threads = 512;

  double tstart, tend;
  tstart = omp_get_wtime();
  cudaMemcpy(d_arr, arr, num_bytes, cudaMemcpyHostToDevice);
  for (int bm_size = 2; bm_size <= size; bm_size *= 2) {
    for (int split_size = bm_size / 2; split_size >= 1; split_size /= 2) {
      bitonic_merge<<<num_blocks, num_threads>>>(d_arr, split_size, bm_size);
    }
  }
  cudaMemcpy(arr, d_arr, num_bytes, cudaMemcpyDeviceToHost);
  tend = omp_get_wtime();

  printf("Time: %f \n", tend - tstart);

  for (int i = 0; i < size - 1; i++) {
    if (arr[i] > arr[i+1]) {
      std::cout << "wrong" << std::endl;
      return;
    }
  }
  std::cout << "correct" << std::endl;

  delete[] arr;
}

int main(int argc, char * argv[]) {
  // srand(time(NULL));
  srand(0);

  if (argc < 2) {
    std::cout << "bitonic <# elements>" << std::endl;
    return 0;
  }
  int size = atoi(argv[1]);
  if (!is_power_of_2(size)) {
    std::cout << "Array size must be power of 2" << std::endl;
    return -1;
  }
  if (size <= 512) {
    std::cout << "Array size must larger than 512" << std::endl;
    return -1;
  }

  cuda_main<double>(size);

  return 0;

}
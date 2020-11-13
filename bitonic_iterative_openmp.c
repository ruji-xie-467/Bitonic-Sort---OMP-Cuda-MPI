#include <iostream> //for std::cout ,std::cin
#include <cstdlib>
#include <time.h>
#include <omp.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "util.h"


template <typename T> 
void compare_and_swap(T * a, T * b, int inc) {
  if ((*a > *b && inc) || (*a < *b && !inc)) {
    T tmp = *a;
    *a = *b;
    *b = tmp;
  }
}

int is_power_of_2(int x) {
  return x > 0 && (((x-1) & x) == 0);
}

template <typename T> 
void bitonic_merge(T * arr, int start, int total_size, int inc) {

  for (int blk_size = total_size; blk_size > 1; blk_size /= 2) {
    int step = blk_size / 2;
    int blk_end = start + total_size;
    // #pragma omp parallel for
    for (int blk_start = start; blk_start < blk_end; blk_start += blk_size) {
      int first_half_end = blk_start + step;
      for (int idx = blk_start; idx < first_half_end; idx++) {
        compare_and_swap<T>(&arr[idx], &arr[idx+step], inc);
      }
    }
  }

}

template <typename T> 
void bitonic_sort(T * arr, int size) {

  for (int n = 2; n < size; n *= 2) {
    #pragma omp parallel for
    for (int start = 0; start < size; start += n*2) {
      bitonic_merge<T>(arr, start, n, 1);
      bitonic_merge<T>(arr, start+n, n, 0);
    }
  }

  bitonic_merge<T>(arr, 0, size, 1);

}


template <typename T> 
int omp_main(int size) {

  // change
  T * arr = gen_data_real<T>(size, 0, std::numeric_limits<T>::max());

  double start, end;
  start = omp_get_wtime();
  bitonic_sort<T>(arr, size);
  end = omp_get_wtime();
  std::cout << "Time: " << end-start << std::endl;

  check_res<T>(arr, size);

  print_arr(arr, 0, size - 1);

  delete[] arr;
}

int main(int argc, char * argv[]) {

  if (argc < 2) {
    std::cout << "bitonic <# elements>" << std::endl;
    return 0;
  }
  int size = atoi(argv[1]);
  if (!is_power_of_2(size)) {
    std::cout << "Array size must be power of 2" << std::endl;
    return -1;
  }
  
  omp_main<double>(size);

  return 0;
}
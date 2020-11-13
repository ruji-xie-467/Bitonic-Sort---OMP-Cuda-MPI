#include <iostream>
#include <cstdlib>
#include <time.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


void print_array(int * arr, int start, int end) {
  for (int i = start; i < end; i++) {
    printf("%d, ", arr[i]);
  }
  printf("\n");
}

void compare_and_swap(int * a, int * b, int inc) {
  if ((*a > *b && inc) || (*a < *b && !inc)) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
  }
}

int findGreatestIntSmallerThan(int target) {
  target--;
  for (int i = 1; i <= 16; i*=2) {
    target |= target >> i;
  }
  return (target + 1) >> 1;
}

void bitonic_merge(int * arr, int start, int end, int inc) {
  int size = end - start;
  if (size <= 1) {
    assert(size != 0);
    return;
  }
  int step = findGreatestIntSmallerThan(size);
  // printf("step: %d, size: %d, start: %d, end: %d\n", step, size, start, end);

  // #pragma omp parallel for
  for (int i = start; i < end - step; i++) {
    compare_and_swap(&arr[i], &arr[i+step], inc);
  }

  // #pragma omp task if(size>1024)
  bitonic_merge(arr, start, start + step, inc);
  bitonic_merge(arr, start + step, end, inc);
  // #pragma omp taskwait

}

void bitonic_sort(int * arr, int start, int end, int inc) {
  int size = end - start;
  if (size <= 1) {
    assert(size != 0);
    return;
  }
  int mid = (end + start) / 2;
  
  #pragma omp task if(size>1024)
  bitonic_sort(arr, start, mid, !inc);
  bitonic_sort(arr, mid, end, inc);
  #pragma omp taskwait
  bitonic_merge(arr, start, end, inc);

}

int main(int argc, char * argv[]) {
    // srand(time(NULL));
  if (argc < 3) {
    printf("bitonic <# elements> <lower bound inclusive> <upper bound exclusive>");
    return 0;
  }
  
  srand(0);
  int size = atoi(argv[1]);
  int lo = atoi(argv[2]);
  int hi = atoi(argv[3]);

  int * arr = new int[size];

  int range = hi - lo;
  for (int i = 0; i < size; i++) {
    arr[i] = rand() % range + lo;
  }


  double start, end;
  start = omp_get_wtime();
  #pragma omp parallel
  {
    #pragma omp single 
    {
      bitonic_sort(arr, 0, size, 1);
    }
  }
  end = omp_get_wtime();
  
  std::cout << "Time taken: " << end-start << std::endl;
  
  
  for (int i = 0; i < size - 1; i++) {
    if (arr[i] > arr[i+1]) {
      printf("wrong\n");
      return 0;
    }
  }

  delete[] arr;

  printf("correct\n");

  return 0;
}

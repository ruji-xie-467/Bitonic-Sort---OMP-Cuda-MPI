#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstdlib>
#include <utility>
#include <math.h>
#include <time.h>
#include <mpi.h>

#include "util.h"

int rank;

int * generate_data(int size, int lo, int hi) {
  int * arr = new int[size];
  int range = hi - lo;
  for (int i = 0; i < size; i++) {
    arr[i] = rand() % range + lo;
  }
  return arr;
}

template <typename T> 
void bitonic_merge(T *arr, int blk_size, int size, int inc) {
  for(int step_size = blk_size / 2; step_size >= 1; step_size /= 2) {
    for(int idx = 0; idx < size; idx++) {
      if(((idx&blk_size) != 0) ^ ((idx&step_size) != 0)) {
        if ((inc && (arr[idx] < arr[idx^step_size])) || (!inc && (arr[idx] > arr[idx^step_size]))) {
          std::swap<T>(arr[idx], arr[idx^step_size]);
        }
      }
    }
  }
}

template <typename T> 
void bitonic_sort(T *arr, int size, int inc) {
  for(int blk_size = 2; blk_size <= size; blk_size *= 2){
    bitonic_merge<T>(arr, blk_size, size, inc);
  }
}

template <typename T> 
T * concat_array(T *a, int a_size, T *b, int b_size) {
  T * res = new T[a_size + b_size];
  memcpy(res, a, sizeof(T) * a_size);
  memcpy(res + a_size, b, sizeof(T) * b_size);
  delete[] a;
  delete[] b;
  return res;
}

int is_power_of_2(int x) {
  return x > 0 && (((x-1) & x) == 0);
}


template <typename T> 
void mpi_main(int data_size) {

  int num_worker;

  T * arr;
  T * scattered_arr;

  double start, end;

	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&num_worker);

  int worker_data_size = data_size / num_worker;

	if (rank == 0) {

    // change
    arr = gen_data_real<T>(data_size, 0, std::numeric_limits<T>::max());

    start = MPI_Wtime();

    scattered_arr = new T[worker_data_size]; 
    MPI_Scatter(arr, worker_data_size, MPI_INT, scattered_arr, worker_data_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    bitonic_sort<T>(scattered_arr, worker_data_size, 1);

  }	else {
    scattered_arr = new T[worker_data_size]; 
    MPI_Scatter(arr, worker_data_size, MPI_INT, scattered_arr, worker_data_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank % 2 == 0) {
      bitonic_sort(scattered_arr, worker_data_size, 1);
    } else {
      bitonic_sort(scattered_arr, worker_data_size, 0);
    }
  }
  
	MPI_Status status;

  for(int merge_worker = 2; merge_worker <= num_worker; merge_worker *= 2) {
    if (rank % merge_worker == 0) {

      T * data_from_worker_about_to_finilize = new T[worker_data_size];
      int worker_about_to_finilize =  rank + merge_worker/2;
      MPI_Recv(data_from_worker_about_to_finilize, worker_data_size, MPI_INT, worker_about_to_finilize, 0, MPI_COMM_WORLD, &status);

      scattered_arr = concat_array<T>(scattered_arr, worker_data_size, data_from_worker_about_to_finilize, worker_data_size);
      worker_data_size *= 2;

      
      if (rank % (merge_worker * 2) == 0) {
        bitonic_merge<T>(scattered_arr, worker_data_size, worker_data_size, 1);
      } else {
        bitonic_merge<T>(scattered_arr, worker_data_size, worker_data_size, 0);
      }

    } else {
      int lead = rank - merge_worker / 2;
      MPI_Send(scattered_arr, worker_data_size, MPI_INT, lead, 0, MPI_COMM_WORLD);
      break;
    }
  }

	if (rank == 0) {

    end = MPI_Wtime();

    printf("Time: %f\n", (end-start));

    // printf("data_size: %d \n", data_size);
    for (int i = 1; i < data_size; i++) {
      // printf("%d, ", scattered_arr[i]);
      if (scattered_arr[i] < scattered_arr[i-1]) {
        MPI_Finalize();
        return;
      }
    }
    printf("correct\n");
	}

	MPI_Finalize();
}

int main(int argc, char **argv)
{

  if (argc < 2) {
    std::cout << "bitonic <# elements>" << std::endl;
    return 0;
  }
  int size = atoi(argv[1]);
  if (!is_power_of_2(size)) {
    std::cout << "Array size must be power of 2" << std::endl;
    return -1;
  }

	MPI_Init(&argc,&argv);
  // change
  mpi_main<double>(size);
  
}
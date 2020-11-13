#include <iostream>
#include <cstdlib>
#include <random>
#include <limits> 
#include <util.h>

template <typename T> 
T * gen_data_int(int size, T lo, T hi) {
  // std::random_device r;
  // std::default_random_engine e1(r());

  std::default_random_engine e1;
  std::uniform_int_distribution<T> uniform_dist(lo, hi);

  T * arr = new T[size];
  
  for (int i = 0; i < size; i++) {
    arr[i] = uniform_dist(e1);
  }
  return arr;
}

template <typename T> 
T * gen_data_real(int size, T lo, T hi) {
  // std::random_device r;
  // std::default_random_engine e1(r());

  std::default_random_engine e1;
  std::uniform_real_distribution<T> uniform_dist(lo, hi);

  T * arr = new T[size];
  
  for (int i = 0; i < size; i++) {
    arr[i] = uniform_dist(e1);
  }
  return arr;
}

void check_res(int * arr, int size) {
  for (int i = 0; i < size - 1; i++) {
    if (arr[i] > arr[i+1]) {
      std::cout << "wrong" << std::endl;
      return;
    }
  }
  std::cout << "correct" << std::endl;
}

template <typename T> 
void print_arr(T * arr, int start, int end) {
  for (int i = start; i < end; i++) {
    std::cout << arr[i] << ", ";
  }

  std::cout << arr[end] << std::endl;
}

int main() {

  double * double_arr = gen_data_real<double>(100, 0, std::numeric_limits<double>::max());
  print_arr(double_arr, 0, 99);
  delete[] double_arr;

  double * int_arr = gen_data_real<double>(100, 0, std::numeric_limits<double>::max());
  print_arr(int_arr, 0, 99);
  delete[] int_arr;
}
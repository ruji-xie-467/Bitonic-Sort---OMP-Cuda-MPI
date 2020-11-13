all: bitonic_recursive_serial bitonic_iterative_serial bitonic_recursive_openmp bitonic_iterative_openmp bitonic_iterative_cuda bitonic_iterative_mpi

OPT = -O2
CC = g++

bitonic_recursive_serial: bitonic_recursive_serial.c 
	$(CC) $(OPT) -std=c++11 -o bitonic_recursive_serial bitonic_recursive_serial.c -fopenmp

bitonic_iterative_serial: bitonic_iterative_serial.c util.h
	$(CC) $(OPT) -std=c++11 -o bitonic_iterative_serial bitonic_iterative_serial.c -fopenmp

bitonic_recursive_openmp: bitonic_recursive_openmp.c 
	$(CC) $(OPT) -std=c++11 -o bitonic_recursive_openmp bitonic_recursive_openmp.c -fopenmp

bitonic_iterative_openmp: bitonic_iterative_openmp.c 
	$(CC) $(OPT) -std=c++11 -o bitonic_iterative_openmp bitonic_iterative_openmp.c -fopenmp

bitonic_iterative_cuda: bitonic_iterative_cuda.cu 
	nvcc --version 
	nvcc $(OPT) $(CUDA_TARGET) -o bitonic_iterative_cuda bitonic_iterative_cuda.cu -Xcompiler -fopenmp

bitonic_iterative_mpi: bitonic_iterative_mpi.c
	mpic++ -std=c++11 -o bitonic_iterative_mpi bitonic_iterative_mpi.c -fopenmp

clean:
	rm bitonic_recursive_serial bitonic_iterative_serial bitonic_recursive_openmp bitonic_iterative_openmp bitonic_iterative_cuda bitonic_iterative_mpi

# Parallel Sort Algorithms Comparision
This project aims to compare various sorting algorithms (quick sort, sample sort and radix sort) in parallel distributed environments. \
OpenMP is used in quick sort as parallel environment, MPI is used in sample sort as distributed environment.

## Quick sort with OpenMP
Quick sort has optimization for small arrays and `std::sort` method is invoked instead of recursive quickSortParallel call.

## Sample sort with MPI

## Radix sort with CUDA


## Testing

### Testing OpenMP (Quick Sort)

1. Compilation: `make openmp_quicksort`
2. Launching: 
```
make run_omp [<THREADS>=<thrdsnum>] [<N>=<amount>]
```

### Testing MPI (Sample Sort)

1. Compilation: `make mpi_samplesort`
2. Launching: `make run_mpi [<PROCS>=<procsnum>] [<N>=<amount>]`
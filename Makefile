N ?= 500000
THREADS ?= 4
PROCS ?= 4

CXX = g++
MPICXX = mpicxx

CXXFLAGS = -O3 -fopenmp
MPIFLAGS = -O3

TARGETS = openmp_quicksort mpi_samplesort

all: $(TARGETS)

openmp_quicksort: openmp_quicksort.cpp
	$(CXX) $(CXXFLAGS) openmp_quicksort.cpp -o openmp_quicksort

mpi_samplesort: mpi_samplesort.cpp
	$(MPICXX) $(MPIFLAGS) mpi_samplesort.cpp -o mpi_samplesort

clean:
	rm -f $(TARGETS)

run_omp: openmp_quicksort
	@echo "Running OpenMP quicksort N = $(N) THREADS = $(THREADS)"
	export OMP_NUM_THREADS=$(THREADS) && ./openmp_quicksort $(N)

run_mpi: mpi_samplesort
	@echo "Running MPI samplesort N = $(N) PROCS = $(PROCS)"    
	mpirun --oversubscribe -np $(PROCS) ./mpi_samplesort $(N)
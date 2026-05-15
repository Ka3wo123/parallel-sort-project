#include <iostream>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <random>
#include "utils/format_time.cpp"
#define DEFAULT_N 100000
#define DEFAULT_NUM_THREADS 4

void quickSortParallel(std::vector<int>& arr, int low, int high, int threshold = 50000) {
    if (low < high) {
	// optimization for small arrays
        if (high - low < threshold) {
            std::sort(arr.begin() + low, arr.begin() + high + 1);
            return;
        }

        int pivot = arr[high];
	    auto it = std::partition(arr.begin() + low, arr.begin() + high, [pivot](int em) {
		return em < pivot;
	});

	int pi = std::distance(arr.begin(), it);
	std::swap(arr[pi], arr[high]);
        
    #pragma omp task shared(arr)
	quickSortParallel(arr, low, pi - 1, threshold);

	#pragma omp task shared(arr)
	quickSortParallel(arr, pi + 1, high, threshold);
    }
}

int main(int argc, char** argv) {
    int n = argc > 1 ? std::atoll(argv[1]) : DEFAULT_N;    
    std::vector<int> data(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000000);

    for (int i = 0; i < n; i++) data[i] = dis(gen);

    double start = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            quickSortParallel(data, 0, n - 1);
        }
    }
    double end = omp_get_wtime();

    std::cout << "================= Quick sort parallel results =================\n";
    std::cout << "Sorting time:\t" << formatTime(end - start) << "\n";
    
    bool is_sorted = std::is_sorted(data.begin(), data.end());
    std::cout << "Is sorted (using std::is_sorted):\t" << (is_sorted ? "Y" : "N") << "\n";

    return 0;
}

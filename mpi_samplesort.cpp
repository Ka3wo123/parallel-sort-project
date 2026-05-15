#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <mpi.h>
#include "utils/format_time.cpp"
#define DEFAULT_N 100000

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = (argc > 1) ? std::atoi(argv[1]) : DEFAULT_N;
    int local_n = n / size;

    std::vector<int> local_data(local_n);
    if (rank == 0) {
        std::vector<int> full_data(n);
        std::mt19937 gen(42);
        std::uniform_int_distribution<> dis(1, 1000000);
        for (int i = 0; i < n; i++) full_data[i] = dis(gen);
        
        MPI_Scatter(full_data.data(), local_n, MPI_INT, local_data.data(), local_n, MPI_INT, 0, MPI_COMM_WORLD);
    } else {
        MPI_Scatter(NULL, 0, MPI_INT, local_data.data(), local_n, MPI_INT, 0, MPI_COMM_WORLD);
    }

    double start = MPI_Wtime();

    std::sort(local_data.begin(), local_data.end());

    std::vector<int> samples(size - 1);
    for (int i = 0; i < size - 1; i++) {
        samples[i] = local_data[(i + 1) * local_n / size];
    }

    std::vector<int> global_samples;
    if (rank == 0) global_samples.resize(size * (size - 1));

    MPI_Gather(samples.data(), size - 1, MPI_INT, global_samples.data(), size - 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::vector<int> pivots(size - 1);
    if (rank == 0) {
        std::sort(global_samples.begin(), global_samples.end());
        for (int i = 0; i < size - 1; i++) {
            pivots[i] = global_samples[(i + 1) * (size - 1)];
        }
    }

    MPI_Bcast(pivots.data(), size - 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::vector<std::vector<int>> buckets(size);
    int current_bucket = 0;
    for (int x : local_data) {
        while (current_bucket < size - 1 && x > pivots[current_bucket]) {
            current_bucket++;
        }
        buckets[current_bucket].push_back(x);
    }

    std::vector<int> send_counts(size);
    for (int i = 0; i < size; i++) send_counts[i] = buckets[i].size();

    std::vector<int> recv_counts(size);
    MPI_Alltoall(send_counts.data(), 1, MPI_INT, recv_counts.data(), 1, MPI_INT, MPI_COMM_WORLD);

    std::vector<int> send_displs(size, 0), recv_displs(size, 0);
    int total_recv = 0;
    std::vector<int> send_buf;
    for (int i = 0; i < size; i++) {
        send_displs[i] = send_buf.size();
        send_buf.insert(send_buf.end(), buckets[i].begin(), buckets[i].end());
        
        recv_displs[i] = total_recv;
        total_recv += recv_counts[i];
    }

    std::vector<int> final_local_data(total_recv);
    MPI_Alltoallv(send_buf.data(), send_counts.data(), send_displs.data(), MPI_INT,
                  final_local_data.data(), recv_counts.data(), recv_displs.data(), MPI_INT, MPI_COMM_WORLD);

    std::sort(final_local_data.begin(), final_local_data.end());

    double end = MPI_Wtime();

    bool local_sorted = std::is_sorted(final_local_data.begin(), final_local_data.end());
    int global_ok = 0;
    int is_ok = local_sorted ? 1 : 0;
    MPI_Reduce(&is_ok, &global_ok, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "================= Sample sort parallel results =================\n";
        std::cout << "Sorting time:\t" << formatTime(end - start) << "\n";
        std::cout << "Is sorted (based on global_ok):\t" << is_ok << "\n";

    }

    MPI_Finalize();
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <mpi.h>

#define MAX_SEND_CHUNK 1000000 // 1 MB per chunk

double getMax(const double *arr, int n);
double getMin(const double *arr, int n);
double getSum(const double *arr, int n);

void printUsage(const char *progName)
{
    printf("Usage: mpirun -np <procs> %s <filename> <runs>\n", progName);
    printf("  <filename> : Path to the input file (default: ../data/medium.txt)\n");
    printf("  <runs>     : Number of runs to perform (default: 100)\n");
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char *filename = "../data/medium.txt";
    int RUNS = 100;

    // Parse command line arguments
    if (argc > 1)
    {
        if (argc == 2 && strcmp(argv[1], "-h") == 0)
        {
            if (rank == 0)
                printUsage(argv[0]);
            MPI_Finalize();
            return 0;
        }
        if (argc >= 2)
            filename = argv[1];
        if (argc >= 3)
        {
            RUNS = atoi(argv[2]);
            if (RUNS <= 0)
            {
                if (rank == 0)
                    fprintf(stderr, "Invalid number of RUNS. Using default 100.\n");
                RUNS = 100;
            }
        }
    }

    // Step 1: Rank 0 reads the entire file
    char *global_buf = NULL;
    long file_size = 0;
    if (rank == 0)
    {
        FILE *file = fopen(filename, "rb");
        if (!file)
        {
            perror("Error opening file");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        rewind(file);

        global_buf = malloc((size_t)file_size + 1);
        if (!global_buf)
        {
            perror("malloc");
            fclose(file);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        size_t bytes_read = fread(global_buf, 1, (size_t)file_size, file);
        global_buf[bytes_read] = '\0';
        file_size = (long)bytes_read;
        fclose(file);
    }

    // Step 2: Broadcast file size to all ranks
    MPI_Bcast(&file_size, 1, MPI_LONG, 0, MPI_COMM_WORLD);

    // Step 3: Calculate domain decomposition (1D block split)
    long chunk_size = file_size / size;
    long remainder = file_size % size;
    long my_start, my_size;
    if (rank < remainder)
    {
        my_size = chunk_size + 1;
        my_start = rank * my_size;
    }
    else
    {
        my_size = chunk_size;
        my_start = rank * chunk_size + remainder;
    }

    // Allocate local buffer
    char *local_buf = malloc((size_t)my_size + 1);
    if (!local_buf)
    {
        perror("malloc local_buf");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Step 4: Distribute data using chunked sends
    if (rank == 0)
    {
        memcpy(local_buf, global_buf + my_start, my_size);
        local_buf[my_size] = '\0';

        for (int r = 1; r < size; r++)
        {
            long r_start, r_size;
            if (r < remainder)
            {
                r_size = chunk_size + 1;
                r_start = r * r_size;
            }
            else
            {
                r_size = chunk_size;
                r_start = r * chunk_size + remainder;
            }

            long sent = 0;
            while (sent < r_size)
            {
                long send_size = (r_size - sent > MAX_SEND_CHUNK) ? MAX_SEND_CHUNK : r_size - sent;
                MPI_Request req;
                MPI_Isend(global_buf + r_start + sent, (int)send_size, MPI_CHAR, r, 0, MPI_COMM_WORLD, &req);
                MPI_Wait(&req, MPI_STATUS_IGNORE);
                sent += send_size;
            }
        }
        free(global_buf);
    }
    else
    {
        long received = 0;
        while (received < my_size)
        {
            long recv_size = (my_size - received > MAX_SEND_CHUNK) ? MAX_SEND_CHUNK : my_size - received;
            MPI_Recv(local_buf + received, (int)recv_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            received += recv_size;
        }
        local_buf[my_size] = '\0';
    }

    if (rank == 0)
    {
        printf("Starting MPI word count with %d processes\n", size);
        printf("File: %s (size: %ld bytes)\n", filename, file_size);
        printf("Running %d iterations...\n\n", RUNS);
    }

    // Allocate timing array
    double *times = malloc(sizeof(double) * RUNS);
    if (!times)
    {
        perror("malloc times");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    long long baseline_words = -1;

    // Step 5: Word counting with halo
    for (int run = 0; run < RUNS; run++)
    {
        MPI_Barrier(MPI_COMM_WORLD);
        double t0 = MPI_Wtime();

        char left_halo = '\0', right_halo = '\0';
        MPI_Request send_requests[2], recv_requests[2];
        int num_send_reqs = 0, num_recv_reqs = 0;

        // Post halo receives
        if (rank > 0)
            MPI_Irecv(&left_halo, 1, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD, &recv_requests[num_recv_reqs++]);
        if (rank < size - 1)
            MPI_Irecv(&right_halo, 1, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD, &recv_requests[num_recv_reqs++]);

        // Post halo sends
        if (rank > 0)
            MPI_Isend(&local_buf[0], 1, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD, &send_requests[num_send_reqs++]);
        if (rank < size - 1)
            MPI_Isend(&local_buf[my_size - 1], 1, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD, &send_requests[num_send_reqs++]);

        // Compute interior
        long long interior_count = 0;
        for (long i = 1; i < my_size; i++)
        {
            if (!isspace((unsigned char)local_buf[i]) && isspace((unsigned char)local_buf[i - 1]))
                interior_count++;
        }

        // Wait halo receives
        if (num_recv_reqs > 0)
            MPI_Waitall(num_recv_reqs, recv_requests, MPI_STATUSES_IGNORE);

        // Compute boundaries
        long long boundary_count = 0;
        if (my_size > 0)
        {
            if (!isspace((unsigned char)local_buf[0]) &&
                (rank == 0 || isspace((unsigned char)left_halo)))
                boundary_count++;
        }

        long long local_count = interior_count + boundary_count;

        // Global reduction
        long long global_count = 0;
        MPI_Reduce(&local_count, &global_count, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

        if (num_send_reqs > 0)
            MPI_Waitall(num_send_reqs, send_requests, MPI_STATUSES_IGNORE);

        times[run] = MPI_Wtime() - t0;

        if (rank == 0)
        {
            if (baseline_words == -1)
                baseline_words = global_count;
            else if (global_count != baseline_words)
                fprintf(stderr, "Warning: word count mismatch (expected %lld, got %lld) at run %d\n",
                        baseline_words, global_count, run);
        }
    }

    // Statistics
    if (rank == 0)
    {
        double max = getMax(times, RUNS);
        double min = getMin(times, RUNS);
        double avg = getSum(times, RUNS) / RUNS;
        printf("✅ Results:\nTotal words: %lld\nProcesses: %d\nMax time: %.6f\nMin time: %.6f\nAvg time: %.6f\nRuns: %d\n",
               baseline_words, size, max, min, avg, RUNS);
    }

    free(local_buf);
    free(times);
    MPI_Finalize();
    return 0;
}

double getMax(const double *arr, int n)
{
    if (n <= 0)
        return 0.0;
    double m = arr[0];
    for (int i = 1; i < n; i++)
        if (arr[i] > m)
            m = arr[i];
    return m;
}

double getMin(const double *arr, int n)
{
    if (n <= 0)
        return 0.0;
    double m = arr[0];
    for (int i = 1; i < n; i++)
        if (arr[i] < m)
            m = arr[i];
    return m;
}

double getSum(const double *arr, int n)
{
    double s = 0.0;
    for (int i = 0; i < n; i++)
        s += arr[i];
    return s;
}

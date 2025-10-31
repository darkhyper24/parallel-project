#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <omp.h>

double getMax(const double *arr, int n);
double getMin(const double *arr, int n);
double getSum(const double *arr, int n);

void printUsage(const char *progName)
{
    printf("Usage: %s <filename> <runs>\n", progName);
    printf("  <filename> : Path to the input file (default: ../data/large.txt)\n");
    printf("  <runs>     : Number of runs to perform (default: 50)\n");
}

int main(int argc, char *argv[])
{

    char *filename = "../data/large.txt"; // Adjust path if needed
    int RUNS = 50;
    if (argc > 1)
    {
        if (argc == 2 && strcmp(argv[1], "-h") == 0)
        {
            printUsage(argv[0]);
            return 0;
        }
        if (argc >= 2)
        {
            filename = argv[1];
        }
        if (argc >= 3)
        {
            RUNS = atoi(argv[2]);
            if (RUNS <= 0)
            {
                fprintf(stderr, "Invalid number of RUNS specified. Using default 50.\n");
                RUNS = 50;
            }
        }
    }

    double min = 0.0;
    double max = 0.0;
    double sum = 0.0;
    double avg = 0.0;
    double *times = malloc(sizeof(double) * RUNS);

    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        perror("Error opening file");
        return 1;
    }

    if (fseek(file, 0, SEEK_END) != 0)
    {
        perror("fseek");
        fclose(file);
        return 1;
    }
    long file_size = ftell(file);
    if (file_size < 0)
    {
        perror("ftell");
        fclose(file);
        return 1;
    }
    rewind(file);

    char *buf = malloc((size_t)file_size + 1);
    if (!buf)
    {
        perror("malloc");
        fclose(file);
        return 1;
    }

    size_t read = fread(buf, 1, (size_t)file_size, file);
    buf[read] = '\0';
    fclose(file);

    long long total_words = 0;
    int nthreads = 1;
    for (int count = 0; count < RUNS; count++)
    {
        total_words = 0;
        double t0 = omp_get_wtime();
#pragma omp parallel
        {
            int tid = omp_get_thread_num();
            int nth = omp_get_num_threads();
#pragma omp single
            nthreads = nth;

            size_t chunk = (read + nth - 1) / nth; // ceil division
            size_t start = tid * chunk;
            size_t end = start + chunk;
            if (end > read)
                end = read;

            long long local_count = 0;
            for (size_t i = start; i < end; ++i)
            {
                unsigned char c = (unsigned char)buf[i];
                if (!isspace(c))
                {
                    if (i == 0)
                    {
                        local_count++;
                    }
                    else if (isspace((unsigned char)buf[i - 1]))
                    {
                        local_count++;
                    }
                }
            }

#pragma omp atomic
            total_words += local_count;
        }

        double t1 = omp_get_wtime();
        times[count] = t1 - t0;
    }
    max = getMax(times, RUNS);
    min = getMin(times, RUNS);
    sum = getSum(times, RUNS);
    avg = sum / RUNS;

    printf("✅ File: %s\n", filename);
    printf("Total words: %lld\n", total_words);
    printf("Threads: %d\n", nthreads);
    printf("max time: %.6f seconds\n", max);
    printf("min time: %.6f seconds\n", min);
    printf("avg time: %.6f seconds\n", avg);
    printf("num of runs: %d\n", RUNS);
    free(buf);
    free(times);
    return 0;
}

double getMax(const double *arr, int n)
{
    if (n <= 0)
        return 0.0;
    double m = arr[0];
    for (int i = 1; i < n; ++i)
        if (arr[i] > m)
            m = arr[i];
    return m;
}

double getMin(const double *arr, int n)
{
    if (n <= 0)
        return 0.0;
    double m = arr[0];
    for (int i = 1; i < n; ++i)
        if (arr[i] < m)
            m = arr[i];
    return m;
}

double getSum(const double *arr, int n)
{
    double s = 0.0;
    for (int i = 0; i < n; ++i)
        s += arr[i];
    return s;
}
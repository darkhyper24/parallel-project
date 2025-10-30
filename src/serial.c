#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

int main() {
    const char *filename = "../data/small.txt"; //this is the path for the data file

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    clock_t start = clock();

    int in_word = 0;
    long word_count = 0;
    char ch;

    while ((ch = fgetc(file)) != EOF) {
        if (isspace(ch)) {
            if (in_word) {
                in_word = 0;
            }
        } else if (!in_word) {
            in_word = 1;
            word_count++;
        }
    }

    clock_t end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

    fclose(file);

    printf(" File: %s\n", filename);
    printf("Total words: %ld\n", word_count);
    printf("Execution time: %.6f seconds\n", time_taken);

    return 0;
}

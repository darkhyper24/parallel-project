#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char word[100];
    int word_count = 0;

    while (fscanf(file, "%99s", word) == 1) {
        word_count++;
    }

    fclose(file);

    printf("Total words: %d\n", word_count);
    return 0;
}

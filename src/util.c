#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROGRESS_BAR_LEN 50

int min(int a, int b) { return a < b ? a : b; }

void progress_bar(int progress) {
    int chars = progress * PROGRESS_BAR_LEN / 100;
    printf("\r[");
    for (int i = 0; i < chars; i++)
        printf("#");
    for (int i = 0; i < PROGRESS_BAR_LEN - chars; i++)
        printf(" ");
    printf("] %d%%", progress);
    fflush(stdout);
}

int fsize(const char *path) {
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "Error: could not find the file %s\n", path);
        exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    rewind(f);
    fclose(f);
    return size;
}

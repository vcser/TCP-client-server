#include "util.h"
#include <stdio.h>

#define PROGRESS_BAR_LEN 50

int min(int a, int b) {
    return a < b ? a : b;
}

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


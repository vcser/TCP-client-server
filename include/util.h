#ifndef UTIL_H
#define UTIL_H

enum flag {
    UNENCRYPTED = 1,
};

int min(int a, int b);

void progress_bar(int progress);

int fsize(const char *path);

#endif

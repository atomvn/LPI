/*
To determine the version of the GNU C Library (glibc) being used in this project, you can use the following code snippet in your C program:
const char* glibc_version = gnu_get_libc_version();
*/
#include <stdio.h>
#include <stdlib.h>
#include <gnu/libc-version.h>

int main() {
    const char* glibc_version = gnu_get_libc_version();
    printf("GNU C Library version: %s\n", glibc_version);
    return EXIT_SUCCESS;
}
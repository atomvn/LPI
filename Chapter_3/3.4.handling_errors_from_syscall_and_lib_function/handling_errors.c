/*
Handling errors from system calls or library function is a critical part of system programming.
To handle errors effectively, you can use the `errno` variable, which is set by system calls and some library functions in the event of an error to indicate what went wrong.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main() {
    int fd = open("/home/noah/WorkPlace/LIP/Chapter_3/3.4/test_file_error.txt", O_RDONLY);
    if (-1 == fd) {
        perror("Error opening file");
        return EXIT_FAILURE;
    } else {
        printf("File opened successfully with file descriptor: %d\n", fd);
    }
    if (-1 == close(fd)) {
        perror("Error closing file");
        return EXIT_FAILURE;
    } else {
        printf("File closed successfully.\n");
    }
    return EXIT_SUCCESS;
}

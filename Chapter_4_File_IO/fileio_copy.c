#include <sys/stat.h>
#include <fcntl.h> // For open()
#include "../lib/tlpi_hdr.h" // For error handling functions

#ifndef BUF_SIZE 
#define BUF_SIZE 1024
#endif

int main(int argc, char *argv[]) {
    int inputFd, outputFd, openFlags;
    mode_t filePerms;
    ssize_t numRead;
    char buf[BUF_SIZE];

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s old-file new-file\n", argv[0]);
    }

    /* Open input and output files*/
    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1) 
        errExit("opening file %s", argv[1]);
    
    openFlags = O_CREAT | O_WRONLY | O_TRUNC; /* Create file if it doesn't exist, write only, truncate to zero length if it does exist */
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; /* rw-r--r-- */
    outputFd = open(argv[2], openFlags, filePerms);
    if (outputFd == -1)
        errExit("opening file %s", argv[2]);
    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
        if (write(outputFd, buf, numRead) != numRead)
            errExit("write() failed");
    }
    if (numRead == -1)
        errExit("read() failed");
    if (close(inputFd) == -1)
        errExit("close input file");
    if (close(outputFd) == -1)
        errExit("close output file");

    exit(EXIT_SUCCESS);
}
/* The tee command reads its standard input until end-of-file, writing a copy of the input
to standard output and to the file named in its command-line argument. (We show
an example of the use of this command when we discuss FIFOs in Section 44.7.)
Implement tee using I/O system calls. By default, tee overwrites any existing file with
the given name. Implement the –a command-line option (tee –a file), which causes tee
to append text to the end of a file if it already exists. (Refer to Appendix B for a
description of the getopt() function, which can be used to parse command-line
options.)
    tee command reads from standard input and writes to standard output and to a file. The program should accept an optional -a flag to append to the file instead of overwriting it. The implementation should use low-level I/O system calls such as open(), read(), write(), and close().
    tee usage: tee [-a] file
    -a: Append to the file instead of overwriting it.
    file: The name of the file to write to.
    The program should read from standard input until EOF, writing the input to both standard output and the specified file. If the -a option is provided, it should append to the file; otherwise, it should overwrite the file.
*/
#include "../lib/tlpi_hdr.h" // For error handling functions
#include <fcntl.h> // For open()
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int opt;
    int optind = 1; // Index of the first non-option argument
    int append_mode =0;
    int open_flags;
    int file_fd;
    char buffer[BUFFER_SIZE];
    ssize_t num_read;

    /* Parse command-line options */
    while ((opt = getopt(argc, argv, "a")) != -1) {
        switch(opt) {
            case 'a':
                append_mode = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-a] file\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Usage: %s [-a] file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* file_name = argv[optind];

    /* Open the file */
    if (append_mode) {
        open_flags = O_WRONLY | O_CREAT | O_APPEND;
    } else {
        open_flags = O_WRONLY | O_CREAT | O_TRUNC;
    }

    file_fd = open(file_name, open_flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (file_fd == -1) {
        errExit("open");
    }

    while ((num_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
        if (write(STDOUT_FILENO, buffer, num_read) != num_read) {
            close(file_fd);
            errExit("write to stdout");
        }

        if (write(file_fd, buffer, num_read) != num_read) {
            close(file_fd);
            errExit("write to file");
        }

        if (num_read == -1) {
            close(file_fd);
            errExit("read");
        }

        if (close(file_fd) == -1) {
            errExit("close file");
        }
    }

    exit(EXIT_SUCCESS);
}
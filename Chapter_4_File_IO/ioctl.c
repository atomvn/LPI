/* 
 * This program demonstrates the use of the ioctl() system call to retrieve the size of the terminal window.
 * It uses the TIOCGWINSZ command to get the number of rows and columns in the terminal.
 * The program prints the retrieved window size to standard output.
 */
#include <../lib/tlpi_hdr.h> // For error handling functions
#include <sys/ioctl.h> // For ioctl()

int main(int argc, char *argv[]) {
    struct winsize ws; // Structure to hold window size information

    /* Send TIOCGWINSZ ioctl to get window size */
    /* STDOUT_FILENO is the file descriptor for standard output, TIOCGWINSZ is the ioctl command, &ws is the address of the structure to hold the result */
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        perror("ioctl");
        return EXIT_FAILURE;
    }

    /* Print the window size */
    printf("Rows: %d, Columns: %d\n", ws.ws_row, ws.ws_col);
    return EXIT_SUCCESS;
}
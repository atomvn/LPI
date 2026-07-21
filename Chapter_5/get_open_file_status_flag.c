#include <fcntl.h>
#include <sys/stat.h>
#include "../lib/tlpi_hdr.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        usageErr("%s file_to_open\n", argv[0]);
    }
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1)
        errExit("fcntl");
    if (flags & O_SYNC)
	printf("Write are synchronized\n");
    if (flags & O_WRONLY)
	printf("Write only\n");

    return (EXIT_SUCCESS);
}

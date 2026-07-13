#include "../../lib/tlpi_hdr.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
    pid_t pid;
    pid = getpid();
    printf("Process ID: %d\n", pid);
    return 0;
}
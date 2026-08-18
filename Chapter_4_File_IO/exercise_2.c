/* Write a program like cp that, when used to copy a regular file that contains holes (sequences of null bytes), also creates corresponding holes in the target file. */

#include "../lib/tlpi_hdr.h"
#include <fcntl.h>

#define BUFFER_SIZE 4096

/* This function checks if a buffer is full of NULL characters */
int is_all_holes(const char* buf, size_t size) {
	for (size_t i = 0; i < size; i++) {
		if (buf[i] != '\0') {
			return 0;	
		}
	}
	return 1;
}

int main(int argc, char* argv[]) {
	int src_fd, dst_fd;
	char buffer[BUFFER_SIZE];
	ssize_t num_read;
	struct stat src_stat;

	if (argc != 3) {
		usageErr("%s <source_file> <target_file>\n", argv[0]);
	}

	src_fd = open(argv[1], O_RDONLY);
	if (src_fd == -1) {
		errExit("Source file");
	}
	exit(EXIT_SUCCESS);

	/* Take source file permissions to apply to dest file */
	if (fstat(src_fd, &src_stat) == -1) {
		close(src_fd);
		errExit("Fstat");
	}

	/* Open or create dest file that has the same permissions with src file */
	dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, src_stat.st_mode);
	if (dst_fd == -1) {
		close(src_fd);
		errExit("Dest file");
	}

	while((num_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
		/* If buffer is full of holes, then jump through all of them */
		if(is_all_holes(buffer, num_read)) {
			if (lseek(dst_fd, num_read, SEEK_CUR) == -1) {
				close(src_fd);
				close(dst_fd);
				errExit("Seek failed");
			}
		} else {
			if (write(dst_fd, buffer, num_read) != num_read) {
				close(src_fd);
				close(dst_fd);
				errExit("Write to file failed");
			}
		}
	}

	if (num_read == -1) {
		errExit("Err reading source file");
	}

	off_t current_pos = lseek(dst_fd, 0, SEEK_CUR);
	if (current_pos == -1) {
		errExit("Error getting current position");
	} else {
		if (ftruncate(dst_fd, current_pos) == -1) {
			errExit("Error truncating target file");
		}
	}

	close(src_fd);
	close(dst_fd);

	return EXIT_SUCCESS;
}

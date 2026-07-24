#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	printf("[WORKER APP].........(PID: %d)\n",getpid());
	for (int i = 0; i < argc; i++) {
		printf("[WORKER APP] Arg[%d]: %s\n", i, argv[i]);
	}
	// printf("[WORKER APP] Sleep for 3 secs\n");
	 sleep(3);
	printf("[WORKER APP] Task done, existing...\n");
	return 0;
}

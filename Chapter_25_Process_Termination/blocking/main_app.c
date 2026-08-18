#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
	printf("[MAIN APP] Program khoi dong (PID: %d)\n", getpid());

	for (int step = 1; step <= 3; step++) {
		printf("[MAIN APP] Doing main task...%d\n", step);
		sleep(1);

		if (step == 2) {
			pid_t pid = fork();
			if (pid == -1) {
				perror("fork failed");
				exit(EXIT_FAILURE);
			}
			/*Child process*/
			if (pid == 0) {
				char *args[] = {"./child_app", "Task_Data_123", "Mode_Fast", NULL};
				execv(args[0], args);
				perror("[CHILD] Exec fail!\n");
				_exit(EXIT_FAILURE);
			} else {
				int status;
				wait(&status);
			}
		}

		printf("[MAIN APP] Continuing parent program...\n");
	}
	printf("[MAIN APP] Main app finished...\n");
	return (EXIT_SUCCESS);
	
}


# Chapter 26: Monitoring child processes

## 26.1. Waiting on a child process
### 26.1.1. The wait() system call
The wait() system call waits for one of the children of the calling process to terminate and returns the termination status of that child in the buffer pointed to by status.
```
#include <sys/wait.h>
pid_t wait(int *status);
Return process ID of terminated child, or 01 on error.
```
#include <sys/wait.h>
pid_t wait(int *status);
Return process ID of terminated child, or -1 on error
```
The wait() system call does the following:
1. If no child of the calling process has yet terminated, the call blocks until one of the children terminates.
2. If status is not NULL, information about how the child terminated is returned in the integer to which status points. 
3. The kernel adds the process CPU times and resource usage statistics to running totals for all children of this parent process.
4. Wait() returns the process ID of the child that has terminated

We can use the following loop to wait for all children of the calling process to terminate:
```
while((childPid = wait(NULL)) != -1)
    continue;
if (errno != ECHILD)
    errExit("wait");
```

The following program demonstrates the use of wait():
```
#include <sys/wait.h> 
#include <time.h> 
#include "curr_time.h"              /* Declaration of currTime() */ 
#include "tlpi_hdr.h" 
int main(int argc, char *argv[]) 
{
    int numDead;       /* Number of children so far waited for */
    pid_t childPid;    /* PID of waited for child */
    int j;
    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s sleep-time...\n", argv[0]);
    setbuf(stdout, NULL);           /* Disable buffering of stdout */
    for (j = 1; j < argc; j++) {    /* Create one child for each argument */
        switch (fork()) {
            case -1:
                errExit("fork");
            case 0:                     /* Child sleep for a while then exits */
                printf("[%s] child %d started with PID %ld, sleeping %s " "seconds\n", currTime("%T"), j, (long) getpid(), argv[j]);
                sleep(getInt(argv[j], GN_NONNEG, "sleep-time"));
                _exit(EXIT_SUCCESS);
        default:                    /* Parent just continues around loop */
        break;
        }
    }
    numDead = 0;
    for (;;) {                      /* Parent waits for each child to exit */
    childPid = wait(NULL);
    if (childPid == -1) {
    if (errno == ECHILD) {
        printf("No more children - bye!\n");
        exit(EXIT_SUCCESS);
    } else {                /* Some other (unexpected) error */
            errExit("wait");
    }
    }
    numDead++;
    printf("[%s] wait() returned child PID %ld (numDead=%d)\n", currTime("%T"), (long) childPid, numDead);
    } 
}
```
Output:
```
./multi_wait 7 1 4
[13:41:00] child 1 started with PID 21835, sleep 7 seconds
[13:41:00] child 1 started with PID 21836, sleep 7 seconds
[13:41:00] child 3 started with PID 21837, sleep 4 seconds
[13:41:01] wait() returned child PID 21836 (numDead =1)
[13:41:04] wait() returned child PID 21837 (numDead =2)
[13:41:01] wait() returned child PID 21835 (numDead =3)
No more children - bye!
```
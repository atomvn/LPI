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
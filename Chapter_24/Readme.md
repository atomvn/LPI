# Chapter 24: Process creation
## 24.1. Overview of fork(), exit(), wait(), and execve()
- The fork() system call allows one process to create a new process, the child obtains copies of the parent's stack, data, heap, and text segments.
- The exit(status) library function terminates a process, return back all process's resources to the kernel.
- The wait(status) system call has two purposes. First, if a child of this process has not yet terminated by calling exit(), then wait() suspends execution of the process until one of its children has terminated. Second, the termination status of the child is return in the status argument.
- The execve(pathname, argv, envp) system call loads a new program into a process's memory. The existing program text is discarded, and the stack, data, and heap segments are freshly created for the new program.

The following provides an overview of how fork(), exit(), wait(), and execve() are commonly used together.
<p align="center">
<img src="../asset/chapter_24/fork_exit_wait_execve.png" alt="fd" width="400" height="400">
</p>

## 24.2. Creating a new process: fork()
The fork() system call creates a new process, the child, which is an almost exact duplicate of the calling process, the parent.
```
#include <unistd.h>
pid_t fork(void);
In parent, returns process ID of child on success, or -1 on error in successfull created child: always return 0
```
After creating a new process, two process exists in parrallel and execution continues from the point where fork() returns.

The following idiom is sometimes employed when calling fork():
```
pid_t childPid;
switch (childPid == for()) {
    case -1:
        /* Handle error */
    case 0: 
        /* Perform actions specific to child */
    default:
        /* Perform actions specific to parent */
}
```

Example of using fork():
```
#include "tlpi_hdr.h
static int idata = 111; /* Allocated in data segment */

int main(int argc, char* argv[]) {
    int istack = 222; /* Allocated in stack segment */
    pid_t childPid;

    switch (childPid = fork()) {
        case -1:
            errExit("fork");
        case 0:
            idata *= 3;
            istack *= 3;
            break;
        default:
            sleep(3); /* Give child a chance to executre */
            break;
    }

    /* Both parent and child come here */
    printf("PID: %ld %s idata=%d istack=%d\n", (long) getpid(), (childPid == 0) ? "(child)" : "(parent)", idata, istack);
    exit(EXIT_SUCCESS);
}
```
Output:
``` 
./t_fork
PID=28557 (child) idata = 333 istack = 666
PID=28556 (parent) idata = 111 istack = 222
```
### 24.2.1. File sharing between parent and child
When a fork() is performed, the child receives duplicates of all of the parent's file descriptors. These duplicates are made in the manner of dup().  

The following program opens a temp file using mkstemp(), and then call fork() to create a child process. The child changes the file offset and open file status flags of the temporary file, and exits. The parent then retrieve the file offset and flags to verify that it can see the changes made by the child. When runnign the program, we see the following:
```
./fork_file_sharing
FIle offset before fork(): 0
O_APPEND flag before fork() is: off
CHild has exited
File offset in parent: 1000
O_APPEND flag in parent is: on
```

```
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"

int main(int argc, char* argv[]) {
    int fd, flags;
    char template[] = "/tmp/testXXXXXX";
    setbuf(stdout, NULL); /* Disable buffering of stdout */
    fd = mkstemp(template);
    if (fd == -1)
        errExit("mkstemp");
        printf("File offset before fork(): %lld\n", (long long) lseek(fd, 0, SEEK_CUR));
    flags = fcntl(fd, F_GETFL);
    if (flags == -1)
        errExit("fcntl - F_GETFL");
    printf("O_APPEND flag before fork() is: %s\n", (flags & O_APPEND) ? "on" : "off");
    switch (fork()) {
        case -1:
            errExit("fork");
        case 0:
            if (lseek(fd, 1000, SEEK_SET) == -1)
                errExit("lseek");
            flags = fcntl(fd, F_GETFL);
            if (flags == -1)
                errExit("fcntl - F_GETFL");
                flags |= O_APPEND;
            if (fcntl(fd, F_SETFL, flags) == -1)
                errExit("fcntl - F_SETFL");
            _exit(EXIT_SUCCESS);
        default:
            if (wait(NULL) == -1)
                errExit("wait");
            printf("Child has exited\n");
            printf("File offset in parent: %lld\n", (long long) lseek(fd, 0, SEEK_CUR));
            flags = fcntl(fd, F_GETFL);
            if (flags == -1)
                errExit(fcntl - F_GETFL);
                printf("O_APPEND flag in parent is: %s\n", (flags & O_APPEND) ? "on" : "off");
                exit(EXIT_SUCCESS);
        
    }
}
```

Duplication of file descriptors during fork:
<p align="center">
<img src="../asset/chapter_24/fd_sharing.png" alt="fd" width="400" height="400">
</p>

### 24.2.2. Memory semantics of fork()
Linux use two techniques to avoid wasteful copying:
- The kernel marks the text segment of each process as read-only. This means that the parent and child can share the same text segment.
- For the pages in the data, heap and stack segments of the parent process, the kernel employs a technique known as copy-on-write.

Description of page tabnles before and after modification of a shared copy-on-write page:
<p align="center">
<img src="../asset/chapter_24/copy_on_write_description.png" alt="fd" width="400" height="400">
</p>

## 24.3. The vfork() system call
Because the unusual semantics of vfork() can lead to some subtle program bugs, its use should normally be avoided, except in the rare cases where it provides worthwhile performance gains.  
Like fork(), vfork() is used by the calling process to create a new child process. Howerver, vfork() is expressly designed to be used in programs where the child performs an immediate exec() call.
```
#include <unistd.h>
pid_t vfork(void);
In parent: return process ID of child on success, or -1 on error; in successfully created child: always return 0
```
Two features distinguish the vfork() system call from fork() and make it more efficient:
- No duplication of virtual memory pages or page tables is done for the child process. Instead the child shares the parent's memory until it either performs a successful exec() or call _exit() to terminate.
- Execution of the parent process is suspended until the child has performed an exec() or _exit().

The following program demonstrates both of the semantic features that distinguish it from vfork(): the child shares the parent's memory and the parent is suspended until the child termninates or call exec(). When ruuning this program, we seek the following outut:
```
./t_vfork
Child executing
Parent executing
istack=666
```
```
#include "tlpi_hdr.h"
int main(int argc, char* argv[]) {
    int istack = 222;
    switch (vfork()) {
        case -1:
            errExit("vfork");
        case 0:
            /* Child executes first, in parent's memory space, even if we sleep for a while parent still is not scheduled */
            sleep(3); 
            write(STDOUT_FILENO, "Child executing\n", 16);
            istack *= 3; // This change will be seen by parent
            _exit(EXIT_SUCCESS);
        default:
            write(STDOUT_FILENO, "Parent executing\n", 17);
            printf("istack=%d\n", istack);
            exit(EXIT_SUCCESS);
    }
}
```

## 24.4. Race conditions after fork()
After a fork(), it is indeterminate which process - the parent or the child - next has access to a CPU.  
The following program demonstrate this interminacy:
```
#include <sys/wait.h>
#include "tlpi_hdr.h"

int main(int argc, char* argv[]) {
    int numChildren, j;
    pid_t childPid;

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [num-children]\n", argv[0]);
    numChildren = (argc > 1) ? getInt(argv[1], GN_GT_O, "num-children") : 1;
    setbuf(stdout, NULL); /* Make stdout unbuffered */
    for (j = 0; j < numChildren; j++) {
        switch (childPid == fork()) {
            case -1:
                errExit("fork");
            case 0:
                printf("%d child\n", j);
                _exit(EXIT_SUCCESS);
            default:
                printf("%d parent\n", j);
                wait(NULL); // Wait for child to terminate
                break;
        }
    }
}
```
Output:
```
./fork_whos_on_first 1
0 parent
0 child
```
It is clear that we can't assume a particular order of execution for the parent and child after a fork(). If we need to guarantee a particular order, we must use some kind of synchronization technique.

## 24.5. Avoiding race conditions by synchronizing with signals
After a fork(), if either process needs to wait for the other to complete an action, then the active process can send a signal after completing the action, the other process waits for the signal.  
The following program demonstrates this technique:  

```
#include <signal.h> 
#include "curr_time.h"                  /* Declaration of currTime() */ 
#include "tlpi_hdr.h" 
#define SYNC_SIG SIGUSR1                /* Synchronization signal */ 

static void handler(int sig)            /* Signal handler - does nothing but return */  
{ 
} 

int 
main(int argc, char *argv[]) 
{
    pid_t childPid;
    sigset_t blockMask, origMask, emptyMask;
    struct sigaction sa;
    setbuf(stdout, NULL);               /* Disable buffering of stdout */
    sigemptyset(&blockMask);
    sigaddset(&blockMask, SYNC_SIG);    /* Block signal */

    if (sigprocmask(SIG_BLOCK, &blockMask, &origMask) == -1)
        errExit("sigprocmask");
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;

    if (sigaction(SYNC_SIG, &sa, NULL) == -1)
        errExit("sigaction");
    switch (childPid = fork()) {
    case -1:
        errExit("fork");

    case 0: /* Child */
        /* Child does some required action here... */
        printf("[%s %ld] Child started - doing some work\n", currTime("%T"), (long) getpid());
        sleep(2);               /* Simulate time spent doing some work */
        /* And then signals parent that it's done */
        printf("[%s %ld] Child about to signal parent\n", currTime("%T"), (long) getpid());
        if (kill(getppid(), SYNC_SIG) == -1)
            errExit("kill");
        /* Now child can do other things... */
        _exit(EXIT_SUCCESS);

    default: /* Parent */
        /* Parent may do some work here, and then waits for child to complete the required action */
        printf("[%s %ld] Parent about to wait for signal\n", currTime("%T"), (long) getpid());

        sigemptyset(&emptyMask);
        if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
            errExit("sigsuspend");

        printf("[%s %ld] Parent got signal\n", currTime("%T"), (long) getpid());

        /* If required, return signal mask to its original state */
        if (sigprocmask(SIG_SETMASK, &origMask, NULL) == -1)
            errExit("sigprocmask");

        /* Parent carries on to do other things... */
        exit(EXIT_SUCCESS);
    } 
} 
```
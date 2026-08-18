# Chapter 25: Process termination

## 25.1. Terminating a process: _exit() and exit()
A process may terminate in two ways: 
1. Abnormal termination: Caused by the delivery of a signal whose default action is to terminate the process.
2. Terminate normally, using the _exit system call.
```
#include <unistd.h>
void _exit(int status);
```
The status argument given to _exit() defines the termination status of the process, which is available to the parent of this process when it calls wait().
Programs generally don't call _exit() directly, but instead call the exit() library function, which performs various actions before calling _exit():
```
#include <stdlib.h>
void exit(int status);
```
The following actions are performed by exit():
- Exit handlers
- The stdio stream buffers are flushed
- The _exit system call is invoked

One other way in which a process may terminate is to return from main(), either explicitly, or implicitly, by falling off the end of the main() function. Performing an explicit return n is generally equivalent to calling exit(n), since the run-timen function that invokes main() uses the return value from main() in a call to exit().

## 25.2. Details of process termination
During both normal and abnormal termination of a process, the following actions occur:
- Open file descriptors, directory streams, message catalog descriptors, and conversion descriptors are closed.
- As a consequence of closing file descriptors, any file locks held by this process are released.
- Any attached system V shared memory segments are detached, and the shm_nattch counter corresponding to each segment is decremented by one.
- For each System V semaphore for which a semadj value has been set by the process, that semadj value is added to the semaphore value.  
- If this is the controlling process for a controlling terminal, then the SIGHUP signal is sent to each process in the controlling terminal’s foreground process group, and the terminal is disassociated from the session. We consider this point further in Section 34.6. 
- Any POSIX named semaphores that are open in the calling process are closed as though sem_close() were called. 
- Any POSIX message queues that are open in the calling process are closed as  though mq_close() were called. 
- If, as a consequence of this process exiting, a process group becomes orphaned and there are any stopped processes in that group, then all processes in the group are sent a SIGHUP signal followed by a SIGCONT signal. We consider this point further in Section 34.7.4. 
- Any memory locks established by this process using mlock() or mlockall() (Section 50.2) are removed. 
- Any memory mappings established by this process using mmap() are unmapped. 

## 25.3. Exit handlers
An exit handler is a programmer-supplied function that is registered at some point during the life of the process and is then automatically called during normal process termination via exit(). Exit handlers are not called if a program calls _exit() directly or if the process is terminated abnormally by a signal.

**Registering exit handlers**
```
#include <stdlib.h>
int atexit(void (*func)(void));
Return 0 on success, or nonzero on error
```
The atexit() function adds func to a list of unction that are called when the process terminates. The func should be defined to take no arguments and return no value, thus having the following general form:
```
void func(void) {
    /* Perfrom some actions */
}

```
It is possible to register multiple exit handlers. When the program invoke exit(), these functions are called in reverse order of registration.
A child process created via fork() inherits a copy of its parent's exit handler registrations. When a process performs an exec(), all exit handler registrations are removed.  
Another library function that supports register exit handler is on_exit():
```
#define _BSD_SOURCE /* Or: #define  _SVID_SOURCE */
#include <stdlib.h>
int on_exit(void (*func)(int, void*), void *arg);
Returns 0 on success, or nonzero on error
```
The func argument of on_exit() is a pointer to a function of the following type:
```
void func(int status, void *arg) {
    /* Perform cleanup actions */
}
```
When called, func() is passed two arguments: the status argument supplied to exit(), and a copy of the arg argument supplied to on_exit() at the time the function was registered.

The following program demonstrates the use of atexit() and on_exit() to register exit handlers:

```
#define _BSD_SOURCE // Get on_exit() declaration from <stdlib.h>
#include <stdlib.h>
#include <tlpi_hdr.h>

static void atexitFunc1 (void) {
    printf("atexit function 1 called\n");
}
static void atexitFunc2(void) {
    printf("atexit function 2 called\n");
}
static void onexitFunc(int exitStatus, void *arg) {
    printf("On_exit function called: status=%d, arg=%ld\n", exitStatus, (long) arg);
}
int main(int argc, char* argv[]) {
    if (on_exit(onexitFunc, (void *) 10) != 0)
        fatal("on_exit 1");
    if (atexit(atexitFunc1) != 0)
        fatal("atexit 1")
    if (atexit(atexitFunc2) != 0)
        fatal("atexit 2")
    if (on_exit(onexitFunc, (void *) 20) != 0)
        fatal("on_exit 2");

    exit(2);
}
```
Output:
```
./exit_handlers
on_exit function called: status 2, arg=20
atexit function 2 called
atexit function 1 called
on_exit function called: status, arg=10
```

## 25.4. Interactions between fork(), stdio Buffers, and exit()
stdio bufffers are maintained in a process's user-space memory. Therefore, these buffers are duplicated in the child by fork().
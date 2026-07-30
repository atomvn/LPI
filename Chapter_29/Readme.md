# Chapter 29: Threads: Introduction

## 29.1. Overview
The threads in a process can execute concurrently. On a multiprocessor system, multiple threads can execute parallel. If one thread is blocked on I/O, other threads are still eligible to execute. All of these threads are independently executing the same program, and they all share the same global memory, including initialized data, uninitialized data, and heap segments.
<p align="center">
<img src="../asset/Chapter_27/four_threads.png" alt="fd" width="600" height="600">
</p>
Besides global memory, threads also share a number of other attributes:

- process ID and parent process ID;
- process group ID and session ID;
- controlling terminal;
- process credentials (user and group IDs);
- open file descriptors;
- record locks created using fcntl();
- signal dispositions;
- file system–related information: umask, current working directory, and root
-irectory;
- interval timers (setitimer()) and POSIX timers (timer_create());
- System V semaphore undo (semadj) values (Section 47.8);
- resource limits;
- CPU time consumed (as returned by times());
- resources consumed (as returned by getrusage()); and
- nice value (set by setpriority() and nice()).

Among the attributes that are distinct for each thread are the following:
- thread ID (Section 29.5);
- signal mask;
- thread-specific data (Section 31.3);
- alternate signal stack (sigaltstack());
- the errno variable;
- floating-point environment (see fenv(3));
- realtime scheduling policy and priority (Sections 35.2 and 35.3);
- CPU affinity (Linux-specific, described in Section 35.4);
- capabilities (Linux-specific, described in Chapter 39); and
- stack (local variables and function call linkage information).

## 29.2. Background details of the Pthread API
**Pthreads data type**  
| Data type | Description |
| --- | --- |
| pthread_t | Thread identifier |
| pthread_mutex_t | Mutex |
| pthread_mutexattr_t | Mutex attributes object |
| pthread_cond_t | Condition variable |
| pthread_condattr_t | Condition variable attributes object |
| pthread_key_t | Key for thread_specific data |
| pthread_once_t | One-time initialization control context |
| pthread_attr_t | Thread attributes object |

**Threads and errno**  
In threaded programs, each thread has its own errno value.

**Return value from Pthread functions**  
All Pthreads functions return 0 on success or a positive value on failure. The failure value is one of the same values that can be placed in errno by traditional UNIX system calls.  
Example:
```
pthread_t *thread;
int s;

s = pthread_create(&thread, NULL, func, &arg);
if (s != 0)
    errExitEN(s, "pthread_create");
```
**Compiling pthreads programs**  

On Linux, programs that use the Pthreads API must be compiled with the cc-pthread option. The effects of this option include the following:
- The _REENTRANT preprocessor macro is defined. The causes the declarations of a few reentrant functions to be exposed.
- The program is linked with the libpthread library. 

## 29.3. Thread creation

The pthread_create() fucntion creates a new thread.
```
#include <pthread.h>
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start)(void *), void *arg);
Return 0 on success, or a positive error number on error.
```
The new thread commences execution by calling the function identified by start with the argument arg (ie, start(arg)). The arg argument is declared as void *, meaning that we can pass a pointer to any type of object to the start function. Typically, arg points to a global or heap variable, but it can also be specified as NULL. If we need to pass multiple arguments to start, then arg can be specified as a pointer to a structure containing the arguments as seperate fields. The return value of start is likewise of type void *, and it can be employed in the same way as the arg argument.  
The thread argument points to a buffer of type pthread_t into which the unique identifier for this thread is copied before pthread_create() returns. This identifier can be used in later Pthreads calls to refer to the thread.   
The attr argument is a pointer to a pthread_attr_t object that specifies various attributes for the new thread. If attr is specified as NULL, then the thread is created with various default attributes, and this is what normally done.  
After a call to pthread_create(), a program has no guarantees about which thread will next be scheduled to use the CPU. Programs that implicitly rely on a particular order of scheduling are open to the same sorts of race conditions.

## 29.4. Thread termination
The execution of a thread terminates in one of the following ways:
- The thread's start function perform a return specifying a return value for the thread.
- The thread calls pthread_exit().
- The thread is canceled using pthread_cancel().
- Any of the threads call exit(), or the main thread perform a return.

The pthread_exit() function terminates the calling thread, and specifies a return value that can be obtained in another thread by calling pthread_join().
```
#include <pthread.h>
void pthread_exit(void *retval);
```
Call pthread_exit() is equivalent to performaing a return in the thread's start fucntion, with the difference that pthread_exit() can be called from any fucntion that has been called by the thread's start fucntion.  
The retval argument specifies the return value for the thread. The value pointed to by retval should not be located on the thread's stack, since the contents of that stack become undefined on thread termination.

## 29.5. Thread IDs
Thread's ID is returned to the caller of pthread_create(), and a thread can obtains its own ID using pthread_self().
```
#include <pthread.h>

pthread_t pthread_self(void);
```

The IDs are useful within applications for the following reasons:
- Various pthreads functions use thread IDs to identify the thread on which they are to act. Examples of such functions include: pthread_join(), pthread_detach(), pthread_cancel(), and pthread_kill().
- It can be useful to tag dynamic data structures with the ID of a particular thread. This can serve to identify the thread that created or "owns" a data structure.

The pthread_equal() function allows us check whether two thread IDs are the same:
```
#include <pthread.h>
int pthread_equal(pthread_t tl, pthread_t t2);
Return nonzero value if t1 and t2 are equal, otherwise 0.
```
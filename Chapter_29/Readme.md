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
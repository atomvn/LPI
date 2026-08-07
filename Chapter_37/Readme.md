# Chapter 37: Daemons
## 37.1. Overview
A daemon is a process with the following characteristics:
- It is long-lived. Often, a daemon is created at system startup and runs until the system is shut down.
- It runs in the background and has no controlling terminal. 
Daemons are written to carry out specific tasks, as illustrated by the following examples:
- cron: a daemon that executes commands at a scheduled time.
- sshd: the secure daemon, which permits login from remote hosts using a secure communications protocol.
- httpd: the HTTP server daemon (Apache), which serves web pages.
- inetd: the Internet superserver daemon, which listen for incoming network connections on specified TCP/IP ports and launches appropriate server programs to handle these connections.  
It is a convention that daemons have names ending with the letter d.

## 37.2. Creating a Deamon
To become a daemon, a program performs the following steps:
1. Perform a fork(), after which the parent exits and the child continues. This step is done for two reasons:
- Assuming the daemon was started from the command line, the parent's termination is noticed by the shell, which then displays another prompt and leaves the child to continue in the background.
- The child process is guaranteed not to be a process group leader, since it inherited its process group ID from its parent and obtained its own nique process ID, which differs from the inherited process group ID. 

2. The child process calls setsid() (Section 34.3) to start a new session and free
itself of any association with a controlling terminal.

3. If the daemon never opens any terminal devices thereafter, then we don’t need
to worry about the daemon reacquiring a controlling terminal. If the daemon
might later open a terminal device, then we must take steps to ensure that the
device does not become the controlling terminal. We can do this in two ways:
- Specify the O_NOCTTY flag on any open() that may apply to a terminal device.
- Alternatively, and more simply, perform a second fork() after the setsid()
call, and again have the parent exit and the (grand)child continue. This
ensures that the child is not the session leader, and thus, according to the
System V conventions for the acquisition of a controlling terminal (which
Linux follows), the process can never reacquire a controlling terminal
(Section 34.4).

4. Clear the process umask (Section 15.4.6), to ensure that, when the daemon creates files and directories, they have the requested permissions.

5. Change the process’s current working directory, typically to the root directory(/). This is necessary because a daemon usually runs until system shutdown; if the daemon’s current working directory is on a file system other than the one containing /, then that file system can’t be unmounted (Section 14.8.2). Alternatively, the daemon can change its working directory to a location where it does its job or a location defined in its configuration file, as long as we know that the file system containing this directory never needs to be unmounted. For
example, cron places itself in /var/spool/cron.

6. Close all open file descriptors that the daemon has inherited from its parent. (A daemon may need to keep certain inherited file descriptors open, so this step is optional, or open to variation.) This is done for a variety of reasons.
Since the daemon has lost its controlling terminal and is running in the back-
ground, it makes no sense for the daemon to keep file descriptors 0, 1, and 2 open if these refer to the terminal. Furthermore, we can’t unmount any file sys-
tems on which the long-lived daemon holds files open. And, as usual, we should close unused open file descriptors because file descriptors are a finite resource.

7. After having closed file descriptors 0, 1, and 2, a daemon normally opens /dev/null and uses dup2() (or similar) to make all those descriptors refer to this device. This is done for two reasons:
- It ensures that if the daemon calls library functions that perform I/O on these descriptors, those functions won’t unexpectedly fail.
- It prevents the possibility that the daemon later opens a file using descriptor 1 or 2, which is then written to—and thus corrupted—by a library function that expects to treat these descriptors as standard output and standard error.

The becomeDaeomon() function takes a bit-mask argument, flags, that allows the caller to selectively inhibit some of the steps, as described in the comments in the following program:

```
#include <syslog.h>

int becomeDaemon(int flags);
Return 0 on success, or -1 on error.

```

```
become_daemon.h

#ifndef BECOME_DAEMON_H /* Prevent double inclusion */
#define BECOME_DAEMON_H

/* Bit-mask values for 'flags' argument of becomeDaemon() */

#define BD_NO_CHDIR 01 /* Don't chdir("/") */
#define BD_NO_CLOSE_FILES 02 /* Don't close all open files */

#define BD_NO_REOPEN_STD_FDS 04 /* Don't reopen stdin, stdout, and stderr to /dev/null */

#define BD_NO_UMASK0 010 /* Don't do a umask(0) */

#define BD_MAX_CLOSE 8192 /* Maximum file descriptors to close if sysconf(_SC_OPEN_MAX) is indeterminate */
int becomeDaemon(int flags);

#endif
```

The implementation of the becomeDaemon() functions:
```
#include <sys/stat.h>
#include <fcntl.h>
#include "become_daemon.h"
#include "tlpi_hdr.h"
int /* Returns 0 on success, -1 on error */
becomeDaemon(int flags)
{
    int maxfd, fd;
    switch (fork()) { /* Become background process */
    case -1: return -1;
    case 0: break; /* Child falls through... */
    default: _exit(EXIT_SUCCESS); /* while parent terminates */
    }

    if (setsid() == -1) /* Become leader of new session */
        return -1;

    switch (fork()) { /* Ensure we are not session leader */
    case -1: return -1;
    case 0: break;
    default: _exit(EXIT_SUCCESS);
    }

    if (!(flags & BD_NO_UMASK0))
        umask(0); /* Clear file mode creation mask */
    if (!(flags & BD_NO_CHDIR))
        chdir("/"); /* Change to root directory */
    if (!(flags & BD_NO_CLOSE_FILES)) { /* Close all open files */
        maxfd = sysconf(_SC_OPEN_MAX);
        if (maxfd == -1) /* Limit is indeterminate... */
            maxfd = BD_MAX_CLOSE; /* so take a guess */
        for (fd = 0; fd < maxfd; fd++)
            close(fd);
    }

    if (!(flags & BD_NO_REOPEN_STD_FDS)) {
        close(STDIN_FILENO); /* Reopen standard fd's to /dev/null */
        fd = open("/dev/null", O_RDWR);
        if (fd != STDIN_FILENO) /* 'fd' should be 0 */
            return -1;
        if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
            return -1;
        if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
            return -1;
    }
    return 0;
}
```

Output:
```
$ ./test_become_daemon
$ ps -C test_become_daemon -o "pid ppid pgid sid tty command"
PID PPID PGID SID TT COMMAND
24731 1 24730 24730 ? ./test_become_daemon
```

Logic of becomeDaemon() function:
```
[Tiến trình C ban đầu]
           │
           ├─► 1. Gọi fork() ──────────► Tiến trình cha thoát (Exit), tiến trình con chạy tiếp
           │
           ├─► 2. Gọi setsid() ────────► Tạo Session mới, tách khỏi Terminal điều khiển (TTY)
           │
           ├─► 3. Gọi chdir("/") ──────► Chuyển Working Directory về Root (tránh khóa USB/Folder)
           │
           ├─► 4. Gọi umask(0) ────────► Reset lại File Mode Creation Mask
           │
           ├─► 5. Đóng stdin/stdout/stderr ──► Chuyển hướng IO sang syslog (tránh in ra Terminal)
           │
           ▼
 [Hạ cánh thành DAEMON hoàn chỉnh chạy ngầm]
```

## 37.3. Guidelines for writing daemons

## 37.4. Using SIGHUP to reinitialize a daemon
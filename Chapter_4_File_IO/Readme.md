# Chapter 4: File I/O: the universal I/O model
## 4.1. Overview
All system calls for performing I/O refer to open files using a file descriptor, a (usually small) nonnegative integer. File descriptors are used to refer to all types of open files, including pipes, FIFOs, sockets, terminals, devices, and regular files.  
By convention, most programs expect to be able to use the three standard file descriptors listed in Table 4-1.
<figure align="center">
    <img src="../asset/Chapter_4/standard_file_descriptor.png" alt="fd" width="300" height="100">
</figure>

The following are the four key system calls for performing file I/O (programming languages and software packages typically employ these calls only indirectly, via I/O libraries):
- fd = open(pathname, flags, mode) opens the file identified by pathname, returning a file descriptor used to refer to the open file in subsequent calls. If the file doesn’t exist, open() may create it, depending on the settings of the flags bitmask argument. The flags argument also specifies whether the file is to be opened for reading, writing, or both. The mode argument specifies the permissions to be placed on the file if it is created by this call. If the open() call is not being used to create a file, this argument is ignored and can be omitted.
- numread = read(fd, buffer, count) reads at most count bytes from the open file referred to by fd and stores them in buffer. The read() call returns the number of bytes actually read. If no further bytes could be read (i.e., end-of-file was encountered), read() returns 0.
- numwritten = write(fd, buffer, count) writes up to count bytes from buffer to the open file referred to by fd. The write() call returns the number of bytes actually written, which may be less than count.
- status = close(fd) is called after all I/O has been completed, in order to release the file descriptor fd and its associated kernel resources.

## 4.2. Universality of I/O
One of the distinguishing features of the UNIX I/O model is the concept of
universality of I/O. This means that the same four system calls—open(), read(), write(), and close()—are used to perform I/O on all types of files, including devices such as terminals.  
Example:
```
$ ./copy test test.old Copy a regular file
$ ./copy a.txt /dev/tty Copy a regular file to this terminal
$ ./copy /dev/tty b.txt Copy input from this terminal to a regular file
$ ./copy /dev/pts/16 /dev/tty Copy input from another terminal
```
## 4.3. Opening a file
The open() system call either opens an existing file or creates and opens a new file.
```
#include <sys/stat.h>
#include <fcntl.h>

int open(const char* pathname, int flags, ... /*mode_t mode*/);
Return file descriptor on success, or -1 on error
```
The file to be opened is identified by the pathname argument. The file to be opened is identified by the pathname argument. If pathname is a symbolic link, it is dereferenced. On success, open() returns a file descriptor that is used to refer to the file in subsequent system calls. If an error occurs, open() returns –1 and errno is set accordingly.

The flags argument is a bit mask that specifies the access mode for the file, using one of the constants shown in Table 4-2.

In the meantime, we’ll just note that the mode argument can be specified as a number (typically in octal) or, preferably, by ORing (|) together zero or more of the bit-mask constants listed in Table 15-4, on page 295.  

Example:
```
/* Open existing file for reading */
fd = open("startup", O_RDONLY);
if (fd == -1)
errExit("open");
/* Open new or existing file for reading and writing, truncating to zero
bytes; file permissions read+write for owner, nothing for all others */
fd = open("myfile", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
if (fd == -1)
errExit("open");
/* Open new or existing file for writing; writes should always
append to end of file */
fd = open("w.log", O_WRONLY | O_CREAT | O_TRUNC | O_APPEND,
S_IRUSR | S_IWUSR);
if (fd == -1)
errExit("open");
```

**`File descriptor number returned by open()`**  
SUSv3 specifies that if open() succeeds, it is guaranteed to use the lowest-numbered unused file descriptor for the process.

### 4.3.1. The open() flags argument
The full set of contants that can be bit-wise ORed (|) in flags. 

<figure align="center">
    <img src="../asset/Chapter_4/value_for_flags.png" alt="fd" width="600" height="600">
</figure>

The constants in Table 4-3 are divided into the following groups:  
- File access mode flags: These are the O_RDONLY, O_WRONLY, and O_RDWR flags described
earlier. They can be retrieved using the fcntl() F_GETFL operation (Section 5.3).
- File creation flags: These are the flags shown in the second part of Table 4-3.
They control various aspects of the behavior of the open() call, as well as
options for subsequent I/O operations. These flags can’t be retrieved or
changed.
- Open file status flags: These are the remaining flags in Table 4-3. They can be
retrieved and modified using the fcntl() F_GETFL and F_SETFL operations (Sec-
tion 5.3). These flags are sometimes simply called the file status flags.


### 4.3.2. Errors from open()
If an error occurs while trying to open the file, open() returns -1, and errno identifies the cause of the error. The following are some possible errors that can occur:
- EACCES
- EISDIR
- EMFILE
- ENFILE
- ENOENT
- EROFS
- ETXTBSY

### 4.3.3. The create() system call
The create() system call was used to create and open a new file.
```
#include <fcntl.h>
int creat(const char* pathname, mode_t mode);
REturns file descriptor, or -1 on error.
```
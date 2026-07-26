# Chapter 27: Program execution
## Chapter 27: Executing a new program: execve()
The execve() system call loads a new program into a process's memory.
```
#include <unistd.h>
int execve(const char *pathname, char* const argv[], char *const envp[]);
Never return on succes, return -1 on error.
```
The pathname argument contains the pathname of the new program to be loaded into the process's memory. The argv arguments specifies the command-line arguments to be passed to the new program. The final argument, envp, specifies the environment list for the new program. If the system call returns, it informs us that an error occured.
Among the errors that may be returned in errno are the following:
- EACCESS: The pathname arguments doesn't refer to a regular file, the file doesn't have execute permission enabled, or one of the directory components of pathname is not searchable.
- ENOENT: The file reffered to by pathname doesn't exitst.
- ENOEXEC: The file referred to by pathname is marked as being executable, but it is not in a recognizable executable format.
- ETXTBSY: The file reffered to by pathname is open for writing by another process.
- E2BIG: The total space required by the argument list and environment list exceeeds the allowed maximum.  

Various library functions, all with names begining with exec, are layered on top the execve() system call.

Example program:

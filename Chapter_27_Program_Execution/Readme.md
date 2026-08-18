# Chapter 27: Program execution
## 27.1. Executing a new program: execve()
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
```
t_execve.c
---------------------------------------------------------
#include "../lib/tlpi_hdr.h"

int main(int argc, char* argv[]) {
    char *argVec[10];
    char *envVec[] = {"GREET=salut", "BYE=adieu", NULL};

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s pathname\n", argv[0]);

    argVec[0] = strrchr(argv[1], '/');

    if (argVec[0] != NULL)
        argVec[0]++;
    else 
        argVec[0] = argv[1];
    argVec[1] = "hello world";
    argVec[2] = "goodbye";
    argVec[3] = NULL;

    execve(argv[1], argVec, envVec);
    errExit("execve");
}
```

```
envargs.c
----------------------------------------
#include "../lib/tlpi_hdr.h"

extern char **environ;

int main(int argc, char* argv[]) {
    int j;
    char **ep;

    for (j = 0; j < argc; j++) {
        printf("argv[%d] = %s\n", j, argv[j]);
    }
    for (ep = environ; *ep != NULL; ep++) {
        printf("environ: %s\n", *ep);
    }
    exit(EXIT_SUCCESS);
}
```

Output:
```
./t_execve ./envargs
argv[0] = envargs
argv[1] = hello world
argv[2] = goodbye
environ: GREET=salut
environ: BYE=adieu
```

## 27.2. The exec() library functions
All of the following functions are layered on top of execve(), and they different from one another and from execve() only in the way in which the program name, argument list, and environment of the new program are specified.

```
#include <unistd.h>

int execle(const char *pathanme, const char *args, ... /* , (char *)NULL, char *const envp[] */);
int execlp(const char *filename, const char *arg, ... /* , (char *) NULL */);
int execvp(const char *filename, char *const argv[]);
int execv(const char* pathname, char *const argv[]);
int execl(const char *pathname, const char *arg, ... /* , (char *) NULL */);
None of the above returns on success, all return -1 on error
```
| Hàm | Loại tham số đường dẫn | Cách truyền danh sách đối số (I vs v) | Nguồn biến môi trường | Cách sử dụng |
| --- | --- | --- | --- | --- |
| execve() (System Call gốc) | Pathname (Cần đường dẫn đầy đủ/tương đối) | Array (v) (Mảng con trỏ char*[]) | envp (e) (Mảng biến môi trường riêng) | Hàm duy nhất là system call thực sự; truyền cả mảng đối số và môi trường custom. |
| execle() | Pathname (Cần đường dẫn đầy đủ/tương đối) | List (l) (Danh sách liệt kê phân tách bởi dấu phẩy) | envp (e) (Mảng biến môi trường riêng) | Truyền đối số dạng danh sách, tham số cuối cùng bắt buộc là NULL, sau đó là mảng envp. |
| execlp() | Filename (p) (Tự tìm trong biến môi trường PATH) | List (l) (Danh sách liệt kê phân tách bởi dấu phẩy) | environ (Dùng chung môi trường tiến trình cha) | Rất tiện khi gọi các lệnh hệ thống (như "ls", "grep") với tham số cố định. |
| execvp() | Filename (p) (Tự tìm trong biến môi trường PATH) | Array (v) (Mảng con trỏ char*[]) | environ (Dùng chung môi trường tiến trình cha) | Thường dùng nhất khi viết Shell/CLI parser vì các đối số đã được phân tích thành mảng argv[]. |
| execv() | Pathname (Cần đường dẫn đầy đủ/tương đối) | Array (v) (Mảng con trỏ char*[]) | environ (Dùng chung môi trường tiến trình cha) | Giống execve(), nhưng kế thừa toàn bộ biến môi trường hiện tại của tiến trình cha. |
| execl() | Pathname (Cần đường dẫn đầy đủ/tương đối) | List (l) (Danh sách liệt kê phân tách bởi dấu phẩy) | enviro (Dùng chung môi trường tiến trình cha) | Cách viết đơn giản nhất khi biết rõ đường dẫn file và số lượng đối số cố định. |




Để phân biệt sự khác biệt giữa các library function trên, ta cần chú ý các điểm sau:
- p (Path): Hàm sẽ nhận vào tên file (Filename, ví dụ "ls"). Nếu không có dấu /, hệ điều hành sẽ tự động tìm file đó trong các thư mục định nghĩa ở biến môi trường PATH. Nếu không có p, bạn bắt buộc phải truyền đường dẫn cụ thể (Pathname, ví dụ "/bin/ls" hoặc "./my_app").

- l (List): Các đối số dòng lệnh được truyền trực tiếp thành danh sách phân tách bằng dấu phẩy trong lời gọi hàm (ví dụ: execl(path, arg0, arg1, arg2, NULL);). Phần tử cuối cùng bắt buộc phải là NULL.

- v (Vector / Array): Các đối số dòng lệnh được đóng gói sẵn vào một mảng các con trỏ char *argv[] (giống như ví dụ argVec trong bài trước). Phần tử cuối cùng của mảng cũng phải là NULL.

- e (Environment): Cho phép bạn truyền một mảng biến môi trường tùy chỉnh char *envp[] (e xịn). Nếu tên hàm không có chữ e, hàm đó sẽ mặc định kế thừa biến môi trường toàn cục environ của tiến trình hiện tại.

### 27.2.1. The PATH environment variable

```
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    if (argc != 2 || strcmp(argv[1]), "--help" == 0)
        usageErr("%s pathname\n", argv[0]);
    execlp(argv[1], argv[1], "hello world", (char *) NULL);
    errExit("exelp");
}
```
Output:
```
$ which echo
/bin/echo
$ ls -l /bin/echo
-rwxr-xr-x    1 root      15428 Mar 19 21:28 /bin/echo 
$ echo $PATH
/home/mtk/bin:/usr/local/bin:/usr/bin:/bin          /bin is in PATH 
$ ./t_execlp echo                 execlp() uses PATH to successfully find echo 
hello world 
```

Cơ chế tìm kiếm file của execlp() / execvp():
- Khi truyền tên file (không chứa dấu /, ví dụ "echo"): Hàm sẽ duyệt lần lượt qua các thư mục được định nghĩa trong biến môi trường PATH (phân tách bởi dấu :) để tìm file thực thi.
- Khi truyền đường dẫn (có chứa ít nhất 1 dấu /, ví dụ "/bin/echo" hoặc "./echo"): Hàm sẽ bỏ qua PATH và tìm chính xác theo đường dẫn đó.
- Nếu không tìm thấy file trong PATH, hàm thất bại và trả về lỗi ENOENT (No such file or directory).

### 27.2.2. Specifying program arguments as a list
```
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    char *envVec[] = {"GREET=salut", "BYE=adieu", NULL};
    char* filename;
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s pathname\n", argv[0]);
    filename = strrchr(argv[1], '/');
    if (filename != NULL)
        filename++;
    else
        filename = argv[1];
    execle(argv[1], filename, "hello world", (char *) NULL, envVec);
    errExit("execle:);
}
```
When we know the number of arguments for an exec at the time we write a program, we can use execle(), execlp(), or execl() to specify the arguments as a list within the function call.

### 27.2.3. Passing the caller's environment to the new program
The execlp(), execvp(), execl(), and execv() functions don’t permit the programmer to explicitly specify an environment list; instead, the new program inherits its environment from the calling process. The following diagram first uses putenv() to make a change to the environment that it inherits from the shell as a result of fork() and then the printenv program is execed to display the values of the the USER and SHELL environment variables.
```
#include <stdlib.h>
#include "tlpi_hdr.h"

int main(int argc, char* argv[]) {
    printf("Initial value of USER: %s\n", getenv("USER"));
    if (putenv("USER=britta") != 0)
        errExit("putenv");
    execl("/usr/bin/printenv", "printenv", "USER", "SHELL", (char *) NULL);
    errExit("execl");
}
```

Output:
```
$ echo $USER $SHELL
blv /bin/bash
# ./t_execl
Initial value of USER: blv
britta
/bin/bash
```
# Chapter 41: Fundamentals of shared library
## 41.1. Object libraries
Object libraries are of two types: static and shared. Shared libraries are the more modern type of object libray, and provide several advantages over static libraries.

## 41.2. Static libraries
Static libraries, also known as archies, were the first type of library to be provided on UNIX systems. They provide the following benefits:
- We can place a set of commonly used object files into a single library file that
can then be used to build multiple executables, without needing to recompile
the original source files when building each application.
- Link commands become simpler. Instead of listing a long series of object files
on the link command line, we specify just the name of the static library. The
linker knows how to search the static library and extract the objects required by
the executable.

** `Creating and maintaining a static library ` **
In effect, a static library is simply a file holding copies of all of the objects file added to it. By conventin, static libraries have names of the form libname.a.   
A static library is created and maintained using the ar(1) command, which has the following general form:
```
$ ar options archieve object-file...
```
The options argument consists of a series of letters, one of which is the operation code, while the others are modifiers that influence the way the operation is carried out. Some commonly used operation codes are the following:
- r (replace): Insert an object file into the archive, replacing any previous object
file of the same name. This is the standard method for creating and updating
an archive. Thus, we might build an archive with the following commands:
```
$ cc -g -c mod1.c mod2.c mod3.c
$ ar r libdemo.a mod1.o mod2.o mod3.o
$ rm mod1.o mod2.o mod3.o
```
- t (table of contents): Display a table of contents of the archive. By default, this
lists just the names of the object files in the archive. By adding the v (verbose)
modifier, we additionally see all of the other attributes recorded in the archive
for each object file, as in the following example:
```
$ ar tv libdemo.a
rw-r--r-- 1000/100 1001016 Nov 15 12:26 2009 mod1.o
rw-r--r-- 1000/100 406668 Nov 15 12:21 2009 mod2.o
rw-r--r-- 1000/100 46672 Nov 15 12:21 2009 mod3.o
```
The additional attributes that we see for each object are, from left to right, its
permissions when it was added to the archive, its user ID and group ID, its size,
and the date and time when it was last modified.
- d (delete): Delete a named module from the archive, as in this example:
```
$ ar d libdemo.a mod3.o
```
**` Using a static library`**
We can link a program against a static library in two ways. The first is to name the
static library as part of the link command, as in the following:
```
$ cc -g -c prog.c
$ cc -g -o prog prog.o libdemo.a
```
Alternatively, we can place the library in one of the standard directories searched
by the linker (e.g., /usr/lib), and then specify the library name (i.e., the filename of the library without the lib prefix and .a suffix) using the –l option:
```
$ cc -g -o prog prog.o -ldemo
```
If the library resides in a directory not normally searched by the linker, we can
specify that the linker should search this additional directory using the –L option:
```
$ cc -g -o prog prog.o -Lmylibdir -ldemo
```

## 41.3. Overview of shared libraries
When a program is built by linking against a static library (or, for that matter, without
using a library at all), the resulting executable file includes copies of all of the
object files that were linked into the program. Thus, when several different execut-
ables use the same object modules, each executable has its own copy of the object
modules. This redundancy of code has several disadvantages:
- Disk space is wasted storing multiple copies of the same object modules. Such
wastage can be considerable.
- If several different programs using the same modules are running at the same
time, then each holds separate copies of the object modules in virtual memory,
thus increasing the overall virtual memory demands on the system.
- If a change is required (perhaps a security or bug fix) to an object module in a
static library, then all executables using that module must be relinked in order
to incorporate the change. This disadvantage is further compounded by the
fact that the system administrator needs to be aware of which applications were
linked against the library.

Shared libraries were designed to address these shortcomings. The key idea of a
shared library is that a single copy of the object modules is shared by all programs
requiring the modules. The object modules are not copied into the linked executable;
instead, a single copy of the library is loaded into memory at run time, when the
first program requiring modules from the shared library is started. When other
programs using the same shared library are later executed, they use the copy of the
library that is already loaded into memory. The use of shared libraries means that
executable programs require less space on disk and (when running) in virtual memory.

## 41.4. Creating and using shared libraries - A first pass
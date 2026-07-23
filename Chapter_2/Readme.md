# Chapter 2: Fundamental conepts
## 2.1. The core operating system: The kernel
The kernel refers to the central software that manages and allocates computer resources (i.e.., the CPU, RAM, and devices)

**Tasks performed by the kernel**
- Process scheduling
- Memory management
- Provision of a file system
- Creation and termination of processes
- Access to devices
- Networking
- Provision of a system call application programming interface

**Kernel mode and user mode**

**Process versus kernel views of the system**

## 2.2. The shell
A shell-command interpreter is a special-purpose program designed to read commands typed by a user and execute apporpriate in response to those commands. 

## 2.3. Users and groups 
Each user on the system is uniquely identified, and users may belong to groups.
**Users**
Several features of a user are stored in /etc/passwd file: login name, user ID (UID), group ID (UID), home directory, login shell.
For security reasons, the password is often stored in the separate shadow password file, which is only readable by privileged users.
**Groups**
Several features of a group are stored in /etc/group: group name, group ID, user list...
**Superuser**
Superuser has user ID 0, normally has the login name root.

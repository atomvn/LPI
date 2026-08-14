# Chapter 56: Sockets: Introduction
## 56.1. Overview
A socket is created using the socket() system call, which returns a file descriptor used to refer to the socket in subsequent system calls:
```
fd = socket(domain, type, protocol);
```
For all applications in this book, protocol is always specified as 0.
**Communication domains**
Communication domain determines:
- the method of communication (ie.., the format of a socket "address")
- the range of communication(ie.., either betwwen applications on the same host or betwwen applications on different hosts connected via a network.)

Modern operation systems support at least the following domains:
- AF_UNIX: allow communication between applications on the same host.
- AF_INET: The IPv4 domain allows communication between applications running on hosts connected via IPv4 network.
- AF_INET6: The IPv6 domain allows communication between applications running on hosts connected via IPv6 network.
 
 The following table summarizes the characteristics of these socket domains:
<p align="center">
<img src="../asset/Chapter_56/domain.png" alt="fd" width="400" height="300">
</p>
 
**Socket types**
In the Internet domain, datagram sockets employ the User Datagram Protocol (UDP), and stream sockets (usually) employ the Transmission Control Protoccol (TCP). Instead of using the terms Internet domain datagram socket and Internet domain stream socket, we just use the term UDP socket and TCP socket, respectively.

**Socket system calls**
- The socket() system call creates a new socket.
- The bind() system call binds a socket to an address. Usually, a server employs
this call to bind its socket to a well-known address so that clients can locate
the socket.
- The listen() system call allows a stream socket to accept incoming connections
from other sockets.

- The accept() system call accepts a connection from a peer application on a listen-
ing stream socket, and optionally returns the address of the peer socket.

- The connect() system call establishes a connection with another socket.

Socket I/O can be performed using the conventional read() and write() system calls, or using a range of socket-specific system calls (eg, send(), recv(), sendto(), recvfrom).

## 56.2. Creating a socket
The socket() system call creates a new socket:
```
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
Returns file descriptor on success, or -1 on error.
```
The domain argument specifies the communication domain for the socket. The type argument specifies the socket type. This argument is usually specified as either SOCK_STREAM, to create a stream socket, or SOCK_DGRAM, to create a datagram socket.The protocol argument is always specified as 0 for the socket types we describe in
this book.

## 56.3. Binding a socket to an address
The bind() system call binds a socket to an address.
```
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
Returns 0 on success, or -1 on error.
```
The sockfd argument is a file descriptor obtained from a previous call to socket(). The
addr argument is a pointer to a structure specifying the address to which this socket
is to be bound. The type of structure passed in this argument depends on the
socket domain. The addrlen argument specifies the size of the address structure.
The socklen_t data type used for the addrlen argument is an integer type specified
by SUSv3.  
Typically, we bind a server’s socket to a well-known address—that is, a fixed
address that is known in advance to client applications that need to communicate
with that server.

## 56.4. Generic socket address structures
The addr and addrlen arguments to bind() require some further explanation. Look-
ing at Table 56-1, we see that each socket domain uses a different address format.

For example, UNIX domain sockets use pathnames, while Internet domain sockets
use the combination of an IP address plus a port number. For each socket domain, a
different structure type is defined to store a socket address. However, because system
calls such as bind() are generic to all socket domains, they must be able to accept
address structures of any type. In order to permit this, the sockets API defines a
generic address structure, struct sockaddr. The only purpose for this type is to cast the
various domain-specific address structures to a single type for use as arguments in
the socket system calls. The sockaddr structure is typically defined as follows:
```
struct sockaddr {
sa_family_t sa_family; /* Address family (AF_* constant) */
char sa_data[14]; /* Socket address (size varies
according to socket domain) */
};
```
This structure serves as a template for all of the domain-specific address structures.
Each of these address structures begins with a family field corresponding to the
sa_family field of the sockaddr structure. (The sa_family_t data type is an integer type
specified in SUSv3.) The value in the family field is sufficient to determine the size
and format of the address stored in the remainder of the structure.

## 56.5. Stream sockets 
<p align="center">
<img src="../asset/Chapter_56/stream_socket.png" alt="fd" width="600" height="600">
</p>

The operation of stream sockets can be explained by analogy with the telephone
system:
1. The socket() system call, which creates a socket, is the equivalent of installing a
telephone. In order for two applications to communicate, each of them must
create a socket.

2. Communication via a stream socket is analogous to a telephone call. One applica-
tion must connect its socket to another application’s socket before communication can take place. Two sockets are connected as follows:
- a) One application calls bind() in order to bind the socket to a well-known
address, and then calls listen() to notify the kernel of its willingness to
accept incoming connections. This step is analogous to having a known
telephone number and ensuring that our telephone is turned on so that
people can call us.

- b) The other application establishes the connection by calling connect(), speci-
fying the address of the socket to which the connection is to be made. This is analogous to dialing someone’s telephone number.
- c) The application that called listen() then accepts the connection using accept().
This is analogous to picking up the telephone when it rings. If the accept() is
performed before the peer application calls connect(), then the accept() blocks
(“waiting by the telephone”).

3. Once a connection has been established, data can be transmitted in both direc-
tions between the applications (analogous to a two-way telephone conversation) until one of them closes the connection using close(). Communication is performed using the conventional read() and write() system calls or via a number of socket-
specific system calls (such as send() and recv()) that provide additional functionality.
# Chapter 61: Sockets: Advanced topic
## 61.3. Socket-specific I/O system calls: recv() and send()
The recv() and send() system calls perform I/O on connected sockets. They provide socket-specific functionality that is not available with the traditional read() and write() system calls.
```
#include <sys/socket.h>
ssize_t recv(int sockfd, void *buffer, size_t length, int flags);

Returns number of bytes received, 0 on EOF, or –1 on error

ssize_t send(int sockfd, const void *buffer, size_t length, int flags);

Returns number of bytes sent, or –1 on error
```
The return value and the first three arguments to recv() and send() are the same as for read() and write(). The last argument, flags, is a bit mask that modifies the behavior of the I/O operation. For recv(), the bits that may be ORed in flags include the following:

- MSG_DONTWAIT: Perform a nonblocking recv(). If no data is available, then instead of blocking, return immediately with the error EAGAIN. We can obtain the same behavior by using fcntl() to set nonblocking mode (O_NONBLOCK) on the socket, with the difference that MSG_DONTWAIT allows us to control nonblocking behavior on a per-call basis.
- MSG_OOB: Receive out-of-band data on the socket. We briefly describe this feature in Section 61.13.1.
- MSG_PEEK: Retrieve a copy of the requested bytes from the socket buffer, but don’t actually remove them from the buffer. The data can later be reread by another recv() or read() call.
- MSG_WAITALL: Normally, a recv() call returns the lesser of the number of bytes requested (length) and the number of bytes actually available in the socket. Specifying the MSG_WAITALL flag causes the system call to block until length bytes have been received. However, even when this flag is specified, the call may return fewer bytes than requested if: (a) a signal is caught; (b) the peer on a stream socket terminated the connection; (c) an out-of-band data byte (Section 61.13.1) was encountered; (d) the received message from a datagram socket is less than length bytes; or (e) an error occurs on the socket. (The MSG_WAITALL flag can replace the readn() function that we show in Listing 61-1, with the difference that our readn() function does restart itself if interrupted by a signal handler.)

All of the above flags are specified in SUSv3, except for MSG_DONTWAIT, which is never-
theless available on some other UNIX implementations. The MSG_WAITALL flag was a later addition to the sockets API, and is not present in some older implementations.

For send(), the bits that may be ORed in flags include the following:
- MSG_DONTWAIT: Perform a nonblocking send(). If the data can’t be immediately transferred (because the socket send buffer is full), then, instead of blocking, fail with the error EAGAIN. As with recv(), the same effect can be achievedby setting the O_NONBLOCK flag for the socket.

- MSG_MORE:  (since Linux 2.4.4)This flag is used with TCP sockets to achieve the same effect as the TCP_CORKsocket option (Section 61.4), with the difference that it provides corking of data on a per-call basis. Since Linux 2.6, this flag can also be used with datagram sockets, where it has a different meaning. Data transmitted in successive
send() or sendto() calls specifying MSG_MORE is packaged into a single datagram that is transmitted only when a further call is made that does not specify this flag. (Linux also provides an analogous UDP_CORK socket option that causes data from successive send() or sendto() calls to be accumulated into a single datagram that is transmitted when UDP_CORK is disabled.) The MSG_MORE flag has no effect for UNIX domain sockets.
- MSG_NOSIGNAL: When sending data on a connected stream socket, don’t generate a SIGPIPE: signal if the other end of the connection has been closed. Instead, the send() call fails with the error EPIPE. This is the same behavior as can be obtained by ignoring the SIGPIPE signal, with the difference that the MSG_NOSIGNAL flag controls the behavior on a per-call basis.
- MSG_OOB: Send out-of-band data on a stream socket. Refer to Section 61.13.1.
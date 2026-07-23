/*
The common header file for the book "The Linux Programming Interface"
*/

#ifndef TLPI_HDR_H
#define TLPI_HDR_H
#include <sys/types.h>  /* Type definitions used by many programs */
#include <sys/stat.h>   /* For struct stat and S_* constants */
#include <stdio.h>      /* Standard I/O functions */
#include <stdlib.h>     /* Prototypes of commonly used library functions,
                            plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>     /* Prototypes for many system calls */
#include <string.h>     /* Commonly used string-handling functions */
#include <errno.h>      /* Declares errno and defines error constants */

#include "get_num.h"
#include "error_functions.h"

typedef enum { FALSE, TRUE } Boolean;  /* Boolean type */

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#endif // TLPI_HDR_H
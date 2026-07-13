#include <stdarg.h>
#include "ename.c.inc"
#include "error_functions.h"  /* Declares error-handling functions. */
#include "tlpi_hdr.h"        /* Declares exit(), and other functions. */

#ifdef __GNUC__
__attribute__ ((__noreturn__))
#endif

/* Terminate the program, possibly dumping core, depending on the value of the
   EF_DUMPCORE environment variable.  If useExit3 is TRUE, call exit() (which
   performs all the normal cleanup actions); otherwise call _exit() (which
   terminates the program without performing any cleanup).  */
static void terminate(Boolean useExit3) {
    char* s;
    /* Dump core if EF_DUMPCORE environment variable is set */
    s = getenv("EF_DUMPCORE");
    if (s != NULL && *s != '\0') {
        abort();
    } else if (useExit3) {
        exit(EXIT_FAILURE);
    } else {
        _exit(EXIT_FAILURE);
    }
}

/* Print a formatted error message and terminate the program.  The first part
   of the message is user-defined; the second part describes the error that
   caused the termination (if any).  If useErr is TRUE, include the string
   strerror(err) in the message.  If flushStdout is TRUE, flush stdout before
   printing the message.  */
static void outputError(Boolean useErr, int err, Boolean flushStdout, const char *format, va_list ap) 
{ 
#define BUF_SIZE 500
    char buf[BUF_SIZE], userMsg[BUF_SIZE], errText[BUF_SIZE];
    vsnprintf(userMsg, BUF_SIZE, format, ap);
    if (useErr)
        snprintf(errText, BUF_SIZE, " [%s %s]", (err > 0 && err <= MAX_ENAME) ? ename[err] : "?UNKNOWN?", strerror(err));
    else
        snprintf(errText, BUF_SIZE, ":");
    snprintf(buf, BUF_SIZE, "ERROR%s %s\n", errText, userMsg);
    if (flushStdout)
        fflush(stdout);       /* Flush any pending stdout */
    fputs(buf, stderr);
    fflush(stderr);           /* In case stderr is not line-buffered */ 
} 

/* Print an error message corresponding to the current 'errno', but DO NOT terminate the program.
   It preserves the original 'errno' value after execution, allowing the program to continue safely. */
void errMsg(const char *format, ...) 
{
    va_list argList;
    int savedErrno;
    savedErrno = errno;       /* In case we change it here */
    va_start(argList, format);
    outputError(TRUE, errno, TRUE, format, argList);
    va_end(argList);
    errno = savedErrno; 
} 

/* Print an error message corresponding to 'errno' and terminate the program using clean termination (exit()).
   Standard way to crash a process cleanly. It flushes stdio buffers before exiting, making it safe for the Parent process. */
void errExit(const char *format, ...) 
{
    va_list argList;
    va_start(argList, format);
    outputError(TRUE, errno, TRUE, format, argList);
    va_end(argList);
    terminate(TRUE); 
}

/* Print an error message corresponding to 'errno' and terminate the program immediately using raw system termination (_exit()).
   Crucial for Child processes (especially after fork/vfork). It DOES NOT flush stdio buffers, preventing interference with the parent's memory.*/
void err_exit(const char *format, ...) 
{
    va_list argList;
    va_start(argList, format);
    outputError(TRUE, errno, FALSE, format, argList);
    va_end(argList);
    terminate(FALSE); 
} 

/* Print an error message using an explicitly passed error number (errnum) instead of reading 'errno', then terminate cleanly via exit().
    Specifically designed for POSIX threads (pthreads), where functions do not set 'errno' but return the error code directly as an integer. */
void errExitEN(int errnum, const char *format, ...) 
{
    va_list argList;
    va_start(argList, format);
    outputError(TRUE, errnum, TRUE, format, argList);
    va_end(argList);
    terminate(TRUE); 
} 

/* Handle a generic critical/fatal application-level error and terminate cleanly.
   Used for general logic errors. Unlike the functions above, it does NOT append any text or information related to system 'errno' codes.*/
void fatal(const char *format, ...) 
{
    va_list argList;
    va_start(argList, format);
    outputError(FALSE, 0, TRUE, format, argList);
    va_end(argList);
    terminate(TRUE); 
}

/* Print a standard command usage syntax error prefix ("Usage: ...") and exit.
   Automatically flushes stdout before printing to stderr, then exits using exit(EXIT_FAILURE). Used for general CLI argument errors.*/
void usageErr(const char *format, ...) 
{
    va_list argList;
    fflush(stdout);           /* Flush any pending stdout */
    fprintf(stderr, "Usage: ");
    va_start(argList, format);
    vfprintf(stderr, format, argList);
    va_end(argList);
    fflush(stderr);           /* In case stderr is not line-buffered */
    exit(EXIT_FAILURE); 
}

/* Print a detailed command-line argument error prefix ("Command-line usage error: ...") and exit.
   Very similar to usageErr(), but provides a more explicit prefix indicating that a specific command-line option/flag or argument value was invalid.*/
void cmdLineErr(const char *format, ...) 
{
    va_list argList;
    fflush(stdout);           /* Flush any pending stdout */
    fprintf(stderr, "Command-line usage error: ");
    va_start(argList, format);
    vfprintf(stderr, format, argList);
    va_end(argList);
    fflush(stderr);           /* In case stderr is not line-buffered */
    exit(EXIT_FAILURE); 
} 
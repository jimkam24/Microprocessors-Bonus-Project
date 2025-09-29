/* Support files for GNU libc.  Files in the system namespace go here.
   Files in the C namespace (ie those that do not start with an
   underscore) go in .c.  */


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include "tim.h"
#include "usart.h"


#undef errno
extern int errno;

#define MAX_STACK_SIZE 0x2000

extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));

void initialise_monitor_handles() {
}

int _getpid(void) {
  return 1;
}

int _kill(int pid, int sig) {
  errno = EINVAL;
  return -1;
}

void _exit (int status) {
  _kill(status, -1);
  while (1) {}
}

int _write(int file, char *data, int len) {
    HAL_UART_Transmit(&huart2, (uint8_t *)data, len, HAL_MAX_DELAY);
    return len;
}

int _close(int file) {
  return -1;
}

int _fstat(int file, struct stat* st) {
  st->st_mode = S_IFCHR;
  return 0;
}

int _isatty(int file) {
  return 1;
}

int _lseek(int file, int ptr, int dir) {
  return 0;
}

int _read(int file, char *data, int len) {
    // Read 'len' number of bytes from UART and store in 'data'
    HAL_StatusTypeDef status = HAL_UART_Receive(&huart2, (uint8_t *)data, len, HAL_MAX_DELAY);

    // Return the number of bytes read
    return (status == HAL_OK) ? len : 0;
}

int _open(char* path, int flags, ...) {
  /* Pretend like we always fail */
  return -1;
}

int _wait(int* status) {
  errno = ECHILD;
  return -1;
}

int _unlink(char* name) {
  errno = ENOENT;
  return -1;
}

int _times(struct tms* buf) {
  return -1;
}

int _stat(char* file, struct stat* st) {
  st->st_mode = S_IFCHR;
  return 0;
}

int _link(char* old, char* new) {
  errno = EMLINK;
  return -1;
}

int _fork(void) {
  errno = EAGAIN;
  return -1;
}

int _execve(char* name, char** argv, char** env) {
  errno = ENOMEM;
  return -1;
}

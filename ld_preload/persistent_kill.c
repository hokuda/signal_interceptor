#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>
#include <errno.h>

void
quit(const char * restrict fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vprintf(fmt, args);
    exit(1);
}

void
main(int argc, char *argv[])
{
  if (argc<2) quit("help: %s <pid>\n", argv[0]);
  int pid = atoi(argv[1]);

  if (pid < 1) quit("invalid pid:%s\n", argv[1]);

  printf("sending SIGSEGV to %d\n", pid);
  if (kill(pid, SIGSEGV)) quit("Failed to kill %d with errno %d\n", pid, errno);
  sleep(1);
}

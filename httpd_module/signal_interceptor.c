#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#define __USE_GNU
#include <dlfcn.h>
#include <time.h>

#include "signal_interceptor.h"

//#define __PRINT_DEBUG
#define SIGNAL SIGSEGV

char *
code2desc (int code)
{
  switch (code)
    {
      case SI_ASYNCNL:	return "SI_ASYNCNL(Sent by asynch name lookup completion)";
      case SI_DETHREAD:	return "SI_DETHREAD(Sent by execve killing subsidiary threads)";
      case SI_TKILL:	return "SI_TKILL(Sent by tkill)";
      case SI_SIGIO:	return "SI_SIGIO(Sent by queued SIGIO)";
      case SI_ASYNCIO:	return "SI_ASYNCIO(Sent by AIO completion)";
      case SI_MESGQ:	return "SI_MESGQ(Sent by real time mesq state change)";
      case SI_TIMER:	return "SI_TIMER(Sent by timer expiration)";
      case SI_QUEUE:	return "SI_QUEUE(Sent by sigqueue)";
      case SI_USER:	return "SI_USER(Sent by kill, sigsend)";
      case SI_KERNEL:	return "SI_KERNEL(Send by kernel);";
    }
  return "unknown";
}

#define BUFSIZE 16+1
static char buf[BUFSIZE];
static char *numerals = "0123456789abcdef";

// convert long to decimal string signal-safely
static char *
long2dec (long i)
{
  long _i = i;
  long idx = BUFSIZE -1;
  buf[idx] = '\0';
  do {
    --idx;
    long r = _i % 10;
    long q = _i / 10;
    buf[idx] = numerals[r];
    _i = q;
  } while(_i);
  return &buf[idx];
}

// convert int to decimal string signal-safely
static char *
int2dec (int i)
{
  return long2dec((long)i);
}

// convert void* to hex string signal-safely
static char *
pointer2hex (void *p)
{
  unsigned long long _p = (unsigned long long)p;
  int idx = BUFSIZE -1;
  buf[idx] = '\0';
  do {
    --idx;
    unsigned long long r = _p % 16;
    unsigned long long q = _p / 16;
    buf[idx] = numerals[r];
    _p = q;
  } while(_p);
  return &buf[idx];
}

// return length of message signal-safely
static int
len(char *message)
{
  int l = 0;
  while (message[l] != '\0') ++l;
  return l;
}

// write message to stderr with the format "key:\tvalue\n" signal-safely
static void *
w(char *key, char *value)
{
  write(2, key, len(key));
  write(2, ":\t", 2);
  if (len(key)<7) write(2, "\t", 1);
  write(2, value, len(value));
  write(2, "\n", 1);
}
    

static char procfile[1024];
static char cmdlinebuf[4096];

static char *
cmdline(int pid)
{
  memcpy(procfile, "/proc/", 6);
  procfile[6] = '\0';
  char *p = int2dec(pid);
  int plen = len(p);
  memcpy(procfile+6, p, plen);
  memcpy(procfile+6+plen, "/cmdline", 8);
  procfile[6+plen+8] = '\0';
  w("procfile",	procfile);
  int fd = open(procfile, O_RDONLY);
  read(fd, cmdlinebuf, 4096);
  return cmdlinebuf;
}

void
my_sigaction (int sig, siginfo_t *info, void *ucontext)
{
  write(2, "____custom sigaction invoked____\n", 34);
  w("epochtime", long2dec((long)time(NULL)));
  w("si_signo",  int2dec(info->si_signo));
  w("si_code",   code2desc(info->si_code));
  w("si_pid",    int2dec(info->si_pid));
  w("si_uid",    int2dec(info->si_uid));
  w("si_addr",   pointer2hex(info->si_addr));
  w("si_status", int2dec(info->si_status));
  w("cmdline",   cmdline(info->si_pid));

  // reset the signal handler to the default
  struct sigaction reset_sa;
  reset_sa.sa_handler = SIG_DFL;
  if (sigaction(SIGNAL, &reset_sa, NULL) == -1) {
    perror("Failed to restore sigaction");
  }
}

#ifdef __PRINT_DEBUG
// Resolve the address to a symbol name
char *
addr2name(void *addr)
{
  Dl_info info;
  
  if (dladdr(addr, &info)) {
    return (char *)info.dli_sname;
  } else {
    fprintf(stderr, "Failed to get symbol information for address: %p\n", addr);
    return NULL;
  }
}

// Debug-print the signal handler's information
void
print_current_sigaction ()
{
  struct sigaction sa;
  
  if (sigaction(SIGNAL, NULL, &sa) == 0) {
    fprintf (stderr, "sa_handler: %p, sa_sigaction: %p, sa_mask: %x, sa_flags: %x, sa_restorer: %p\n",
             sa.sa_handler,
             sa.sa_sigaction,
             sa.sa_mask,
             sa.sa_flags,
             sa.sa_restorer
             );
    if (sa.sa_handler == SIG_DFL) {
      printf("The default handler has been set\n");
    } else if (sa.sa_handler == SIG_IGN) {
      printf("The signal is ignored\n");
    } else {
      printf("A user-defined handler has been set\n");
      printf("handler address: %0x\n", sa.sa_sigaction);

      char *name;
      if (name = addr2name(sa.sa_sigaction)) {
        fprintf(stderr, "Address: %p(%p), Symbol: %s\n", sa.sa_sigaction, my_sigaction, name);
      } else {
        fprintf(stderr, "Failed to get symbol information for address: %p\n", sa.sa_sigaction);
      }
    }
  } else {
    perror("sigaction");
  }
}
#endif // __PRINT_DEBUG

void
set_custom_sigaction()
{
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = my_sigaction;

  int ret = sigaction(SIGNAL, &sa, NULL);
  if (ret < 0) {
    perror ("sigaction failed");
    exit (EXIT_FAILURE);
  }
  //fprintf (stderr, "[signal_interceptor] HHHHHHHHHHHHHHHHHHHH pid: %d\n", getpid());
}

__attribute__((constructor))
void
do_something_before_main()
{
#ifdef __PRINT_DEBUG
  print_current_sigaction();
#endif

  set_custom_sigaction();

#ifdef __PRINT_DEBUG
  print_current_sigaction();
#endif
}
/*
*/

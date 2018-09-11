
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

/* zprintf : funzione che simula il comportamento della fprintf attraverso chiamate di sistema */
void zprintf(int fd, const char *fmt, ...) 
{
   static char msg[1024];
   int n;
   va_list ap;
   
   va_start(ap, fmt);
   n = vsnprintf(msg, 1024, fmt, ap);
   n = write(fd, msg, n);
   va_end(ap);
}

typedef int pipe_t[2];

void handler(int signo)
{
   zprintf(1, "\n\n Io sono il nuovo handler di SIGPIPE \n\n");
}

int main(int argc, char **argv)
{
   pipe_t piped;
   int buffer = 1;

   pipe(piped);

   close(piped[0]);
   switch(fork())
   {
      case 0:
              signal(SIGPIPE, handler);
              write(piped[1], &buffer, sizeof(int));
	      break;

      case -1:
	      break;
   }

   exit(EXIT_SUCCESS);
}


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

int main(int argc, char **argv)
{
   pipe_t p;
   pid_t pid;
   int buffer, status;

   pipe(p);
   close(p[0]);

   switch(fork())
   {
      case 0:
         buffer = 1;
	 write(p[1], &buffer, sizeof(buffer));

	 exit(EXIT_SUCCESS);
	 break;

      case -1:
	 break;
   }

   wait(&status);
   if (WIFSIGNALED(status))
   {
      zprintf(1, " Attenzione : il processo è stato interrotto dal segnale %d\n", WTERMSIG(status));
   }

   exit(EXIT_SUCCESS);
}

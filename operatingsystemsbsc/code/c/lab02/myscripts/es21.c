/* Includo le librerie */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "utils.h"

/* Funziona apposita contenente il codice dei figli, atta a separare quest'ultimo dal codice del padre */
void child()
{
   zprintf(1, "\n\n[%d] Hello, i'm the child!\n", getpid());   
   exit(0);
}

int main(int argc, char **argv)
{
   if (argc < 1)
   {
      zprintf(1, " Utilizzo: %s ", argv[0]);
      exit(1);
   }
   
   zprintf(1, "[%d] Hello, i'm the father!\n\n", getpid());
   int pid = fork();
   switch(pid)
   {
      case 0:
	      child();
	      break;

      case -1:
	      zprintf(1, "Errore in fase di creazione del processo");
	      exit(2);
	      break;
   }

   int status;
   pid = wait(&status);
   zprintf(1, "\n\n[%d] Figlio terminato con codice di uscita %d e PID %d\n\n", getpid(), WEXITSTATUS(status), pid);

   
   return EXIT_SUCCESS;
}

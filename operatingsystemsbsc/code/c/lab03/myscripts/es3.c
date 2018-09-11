
/* Includo le librerie */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "utils.h"

#define N 16

typedef int pipe_t[2];

/* Funziona apposita contenente il codice dei figli, atta a separare quest'ultimo dal codice del padre */
void child(pipe_t p)
{
   close(p[1]);
   int buffer, array[N]; 
   int i;

   for(i = 0; i < N; i++)
   {
      if (read(p[0], &buffer, sizeof(int)) != sizeof(int))
      {
         zprintf(2, " [%d] Errore : lettura del numero %d non riuscita\n\n", getpid(), i);
	 exit(3);
      }

      array[i] = buffer;
   }

   for (i = 0; i < N; i++) zprintf(1, "%d ", array[i]);
   zprintf(1, "\n");

   /* Tutto è andato come previsto: esco dal processo figlio con codice zero */
   exit(EXIT_SUCCESS);
}

void father(pipe_t p)
{
   int array[N], status;
   memset(array, 0, N * sizeof(int));

   close(p[0]);
   write(p[1], array, N * sizeof(int));
   wait(&status);
   if(WEXITSTATUS(status) != EXIT_SUCCESS)
   {
      zprintf(2, " Errore : passaggio dei valori al processo figlio fallito\n\n");
      exit(2);
   }
}

int main(int argc, char **argv)
{
      /* Variabile per gestire i pid ritornati dalle varie funzioni */
   pid_t pid;

   /* Variabile apposita per contenere i file descriptor della pipe */
   pipe_t piped;

   /* Effettuo pipe e fork */
   pipe(piped);
   pid = fork();
   switch(pid)
   {
      case 0:
              child(piped);
	      break;

      case -1:
              zprintf(2, " Errore : fork non riuscita\n\n");
	      exit(1);
	      break;
   }

   father(piped);

   exit(EXIT_SUCCESS);
}

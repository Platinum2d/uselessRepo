
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
void child(int index)
{
   zprintf(1, "\n[%d] %d\n", getpid(), index);
   exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
   pid_t pid;
   int status;
   int opt;
   /* Variabile per contenere il numero di processi da creare */
   int n;
   /* Variabile di iterazione dei cicli for */
   int i;

   /* Controllo il numero di parametri: devono essere almeno 2, incluso l'elemento di posizione 0 */
   if (argc < 2)
   {
      zprintf(2, " Utilizzo : %s -n intero_positivo\n", argv[0]);
      exit(1);
   }
   
   /* Prelevo i parametri usando getopt */
   for (;;)
   {
      opt = getopt(argc, argv, "n:");
      if (opt == -1) break;

      switch (opt)
      {
         case 'n':
		 n = atoi(optarg);
		 break;
      }
   }

   /* Inizio a creare n processi, in cui, per ognuno, richiamo la funzione del figlio */
   for (i = 0; i < n; i++)
   {
      pid = fork();
      switch(pid)
      {
         case 0:
      		 child(i);
		 break;

	 case -1:
		 zprintf(2, " Errore : fork fallita ");
		 exit(2);
		 break;
      }
   }

   zprintf(1, "\n\n[%d] Inizio del padre...\n", getpid());
   for (i = 0; i < n; i++)
   {
      pid = wait(&status);
      zprintf(1, "\n[%d] %d terminato con codice %d\n", getpid(), pid, WEXITSTATUS(status));
   }

   return EXIT_SUCCESS;
}

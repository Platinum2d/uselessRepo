
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

   /* Tutto è andato come previsto: esco dal processo figlio con codice zero */
   exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
   /* Variabile per gestire i pid ritornati dalle varie funzioni */
   pid_t pid;
   
   /* Variabile per gestire il format di ritorno della wait */
   int status;

   /* Variabile indice dei cicli for */
   int i;

   int opt, n;

   /* Effettuo il controllo sul numero dei parametri: devono essere almeno n (incluso l'elemento di posizione 0)*/
   if (argc < 3)
   {
      zprintf(2, "\n Utilizzo : %s -n numero_intero \n", argv[0]);
      exit(1);
   }

   /* Prelevo i parametri */
   for (;;)
   {
      opt = getopt(argc, argv, "n:");
      if (opt == -1) break;
      switch(opt)
      {
         case 'n':
                 n = atoi(optarg);
		 break;
      }
   }

   /* Alloco un'area di memoria apposita per contenere i pid dei processi */
   pid_t *pids = (pid_t *) malloc(n * sizeof(pid_t));

   /* Eseguo la fork per ogni n */
   for (i = 0; i < n; i++)
   {
      pids[i] = fork();
      switch(pids[i])
      {
	 /* Eseguo il codice del figlio in quanto la fork è stata eseguita con successo e memorizzo il pid */
         case 0:
                 child(i);
		 break;

         /* La fork non è stata eseguita: segnalo l'errore ed esco */
	 case -1:
		 zprintf(2, "\n Errore : fork fallita\n");
		 exit(2);
		 break;

      }
   }
   
   zprintf(1, "\n[%d] Comincia il padre...\n", getpid());
   /* Comincio ad attendere a ritroso i processi figli */
   for (i = n - 1; i >= 0; i--)
   {
      waitpid(pids[i], &status, WNOHANG);
      zprintf(1, "\n[%d] Terminato il processo %d con valore di uscita %d\n", getpid(), pids[i], WEXITSTATUS(status));
   }

   free(pids);
   exit(EXIT_SUCCESS);
}

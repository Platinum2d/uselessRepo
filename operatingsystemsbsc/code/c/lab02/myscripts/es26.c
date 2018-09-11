
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
void child(char *command)
{
   zprintf(1, "[%d] Esecuzione del figlio...\n", getpid());
   execlp(command, command, (char *)0);
   
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

   /* Effettuo il controllo sul numero dei parametri: devono essere almeno n (incluso l'elemento di posizione 0)*/
   if (argc < 2)
   {
      zprintf(2, " Utilizzo : %s eseguibile\n\n", argv[0]);
      exit(1);
   }
   
   pid = fork();
   switch(pid)
   {
      case 0:
              child(argv[1]);
	      break;

      case -1:
	      zprintf(2, " Errore : fork non riuscita\n\n");
	      exit(2);
	      break;
   }
   
   pid = wait(&status);
   if (pid < 0)
   {
      zprintf(2, " Errore : il processo figlio non è terminato correttamente\n\n");
      exit(3);

   }
   else
   {
      zprintf(1, "\n\n[%d] Il processo figlio è terminato con codice %d\n\n", getpid(), WEXITSTATUS(status));
   }
   
   
   exit(EXIT_SUCCESS);
}

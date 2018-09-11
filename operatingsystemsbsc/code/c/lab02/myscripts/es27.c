
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
void child(char *command, char **arguments)
{
   zprintf(1, "[%d] Eseguo il figlio...\n", getpid());
   /* Richiamo il comando, passando gli opportuni parametri */
   execvp(command, arguments);
   
   /* Tutto è andato come previsto: esco dal processo figlio con codice zero */
   exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
   /* Controllo il numero dei parametri */
   if (argc < 2)
   {
      zprintf(2, " [%d] Utilizzo : %s eseguibile [parametri]\n\n", getpid(), argv[0]);
      exit(1);
   }

   /* Variabile per gestire i pid ritornati dalle varie funzioni */
   pid_t pid;
   
   /* Variabile per gestire il format di ritorno della wait */
   int status;

   /* Variabile indice dei cicli for */
   int i;
   
   /* Alloco lo spazio di memoria per contenere i parametri passati */
   char **args = malloc((argc - 1) * sizeof(char*));

   /* Prelevo i parametri e li metto in un'area di memoria apposita, così da escludere il primo */
   for (i = 1; i <= argc; i++)
   {
      args[i - 1] = argv[i];
   }
   
   /* Eseguo la fork e ne controllo l'esito */
   pid = fork();
   switch(pid)
   {
      case 0:
	      /* Eseguo il codice del figlio in caso di fork riuscita */
	      zprintf(1, "[%d] Eseguo il figlio : comando %s...\n", getpid(), argv[1]);
	      execvp(argv[1], args);
	      exit(0);
	      break;

      case -1:
	      zprintf(2, " Errore : fork non riuscita\n\n");
	      exit(2);
	      break;
   }

   zprintf(1, "[%d] Eseguo il padre...\n", getpid());
   wait(&status);
   zprintf(1, "[%d] Figlio terminato con codice %d\n\n", getpid(), WEXITSTATUS(status));

   free(args);
   exit(EXIT_SUCCESS);
}

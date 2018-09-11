
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
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

void handler1(int signo)
{
   zprintf(1, "\n[%d] Segnale utente 1!\n", getpid());
   signal(SIGUSR1, handler1);
}

void handler2(int signo)
{
   exit(EXIT_SUCCESS);
}

/* main : funzione contenente il codice del processo padre */
int main(int argc, char **argv) 
{
   /* variabili di iterazione dei cicli */
   int i, sc;

   int status; /* gestione del valore di uscita dei processi figli*/
   pid_t *pids; /* [GENERIC] variabile/array per gestire il/i pid creato/i tramite fork() */
   int child_n; /* variabili atte a contenere il numero di processi figli e nipoti da generare */

   int min_argc = 2; /* valore minimo di argc */
   
   /* controllo che il numero dei parametri passati allo script sia appropriato */
   if (argc < min_argc) 
   {
      /* stampo tutti gli errori sul file avente file descriptor 2, ovvero su stderror */
      zprintf(2, "\t[%d, padre] Utilizzo : %s numero_figli\n\n", getpid(), argv[0]);
      exit(1);
   }
   
   /* stabilisco il numero di processi da creare (tanti quanti i file passati) */
   child_n = atoi(argv[1]);

   pids = (pid_t *) malloc(child_n * sizeof(pid_t));

   /* [GENERIC] controllo i parametri passati a linea di comando */
   if (child_n <= 0)
   {
      zprintf(2, "\t[%d, padre] Errore : il numero dei processi figli deve essere positivo\n\n", getpid());
      exit(2);
   }

   signal(SIGUSR1, handler1);
   signal(SIGUSR2, handler2);

   for (i = 0; i < child_n; i++)
   {
      /* controllo l'esito della fork() */
      switch(pids[i] = fork())
      {
         case 0:
	    /* codice del figlio */
            for(;;) pause();
	    break;

	 /* caso di fork() fallita */
	 case -1:
	    zprintf(2, "\t[%d, padre] Errore : fork() fallita\n\n", getpid());
	    exit(5);
	    break;
      }
   }

   /* codice del padre */

   /* azioni del padre */
   for (;;)
   {
      zprintf(1, "\nCosa vuoi fare?\n1)SIGUSR1\n2)SIGUSR2\n\nSeleziona la tua destinazione : ");
      scanf("%d", &sc);
      switch(sc)
      {
         case 1:
		 for (i = 0; i < child_n; i++)
		    kill(pids[i], SIGUSR1);
		 break;

	 case 2: 
                 for (i = 0; i < child_n; i++)
		    kill(pids[i], SIGUSR2);
                 
		 zprintf(1, "\nEsco...\n");
		 exit(EXIT_SUCCESS);
		 break;
      }
   }
}

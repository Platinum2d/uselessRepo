/* inclusione delle librerie */
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

/* struttura di dati richiesta */
typedef struct mstruct{
   long int c1;
   int c2;
   long int c3;
} mstruct;

/* funzione di stampa di una singola struttura dati */
void printstruct(mstruct s)
{
   zprintf(1, " c1 = %ld, c2 = %d, c3 = %ld ", s.c1, s.c2, s.c3);
}

/* tipo di dato apposito per semplificare l'utilizzo delle pipe multiple */
typedef int pipe_t[2];

/* main : funzione contenente il codice del processo padre */
int main(int argc, char **argv) 
{
   /* variabili locali */

   int i, j; /* variabili di iterazione dei cicli */
   mstruct bufferchild, bufferfather; /* buffer di lettura e scrittura */
   int status; /* gestione del valore di uscita dei processi figli*/
   pid_t pid; /* variabile di gestione di un singolo pid */
   int child_n; /* variabili atte a contenere il numero di processi figli e nipoti da generare */
   pipe_t *p; /* variabile puntatore alla zona di memoria contenente tutte le pipe di comunicazione figlio --> padre */
   int min_argc = 3; /* valore minimo di argc */
   
   /* controllo che il numero dei parametri passati allo script sia appropriato */
   if (argc < min_argc) 
   {
      /* stampo tutti gli errori sul file avente file descriptor 2, ovvero su stderror */
      zprintf(2, "\t[%d, padre] Utilizzo : %s file_1 file_2 [file_3] ... [file_n]\n\n", getpid(), argv[0]);
      exit(1);
   }
   
   /* stabilisco il numero di processi da creare (tanti quanti i file passati) */
   child_n = argc - 1;

   /* alloco la zona di memoria dedicata alle pipe tra processo padre e processi figli, con controllo dell'errore */
   p = (pipe_t *) malloc(sizeof(pipe_t) * child_n);
   if (p == NULL) 
   {
      zprintf(2, "\t[%d, padre] Errore : malloc() fallita \n\n", getpid());
      exit(2);
   }
   
   /* inizializzo le pipeline allocate attraverso la primitiva pipe() */
   for (i = 0; i < child_n; i++) 
   { 
      if (pipe(p[i]) < 0) 
      { 
         zprintf(2, "\t[%d, padre] Errore : pipe() fallita \n\n", getpid());
         exit(3);
      }
   }

   /* creo i processi figli */
   for (i = 0; i < child_n; i++)
   {
      /* controllo l'esito della fork() */
      switch(fork())
      {
         case 0:
	    /* codice del figlio */
            /* chiudo i lati della pipe non utilizzati dal figlio */
	    for (j = 0; j < child_n; j++)
	    {
	       close(p[j][0]);
	       if (j != i) close(p[j][1]);
	    }

            /* azioni del figlio */
	    bufferchild.c1 = bufferchild.c2 = bufferchild.c3 = 666;
	    write(p[i][1], &bufferchild, sizeof(mstruct));

            /* termino */
	    exit(EXIT_SUCCESS);
	    break;

	 /* caso di fork fallita */
	 case -1:
	    zprintf(2, "\t[%d, padre] Errore : fork() fallita\n\n", getpid());
	    exit(4);
	    break;
      }
   }

   /* codice del padre */
   /* chiudo i lati della pipe inutilizzati dal processo padre */
   for (i = 0; i < child_n; i++)
   {
      close(p[i][1]);
   }

   /* azioni del padre */
   zprintf(1, "\n");
   for (i = 0; i < child_n; i++)
   {
      read(p[i][0], &bufferfather, sizeof(mstruct));
      zprintf(1, "[%d, padre] ricevuto : ", getpid());
      printstruct(bufferfather);
      zprintf(1, "\n");
   }

   zprintf(1, "\n\n[%d, padre] Elaborazione terminata\n\n", getpid());
   /* attendo la terminazione dei processi figli */
   for (i = 0; i < child_n; i++)
   {
      if ((pid = wait(&status)) < 0)
      {
         zprintf(2, "\t[%d, padre] Errore : wait() fallita\n\n");
	 exit(5);
      }
      zprintf(1, "[%d, padre] Il processo con pid %d è terminato con codice %d\n", getpid(), pid, WEXITSTATUS(status));
   }
   exit(EXIT_SUCCESS); 
}

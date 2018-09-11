/* inclusione delle librerie */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

/* zprintf: funzione che simula il comportamento della fprintf attraverso le primitive di sistema */
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
}mstruct;

/* funzione di stampa di una struttura dati */
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
   mstruct bufferchild, buffergrandson, bufferfather; /* buffer di lettura e scrittura */
   int status; /* gestione del valore di uscita dei processi figli*/
   pid_t pid; /* variabile gestione di un singolo pid */
   int child_n; /* variabili atte a contenere il numero di processi figli e nipoti da generare */
   pipe_t *p; /* variabile puntatore alla zona di memoria contenente le pipe di comunicazione figlio --> padre */
   pipe_t p2; /* singola pipe di comunicazione nipote --> figlio */
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
         zprintf(2, "\t[%d, padre] Errore : pipe() fallita \n\n");
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

	    /* genero la pipe di comunicazione tra figlio e nipote */
	    if (pipe(p2) < 0)
	    {
	       zprintf(2, "\t[%d, figlio] Errore : pipe() fallita\n\n", getpid());
	       exit(4);
	    }

	    /* creo il nipote, con controllo dell'errore */
	    switch(fork())
	    {
	       case 0:
		  /* codice del nipote */
		  /* chiudo i lati della pipe inutilizzati */
                  close(p2[0]);

		  /* azioni del nipote */
		  buffergrandson.c2 = buffergrandson.c1 = buffergrandson.c3 = 666;
		  write(p2[1], &buffergrandson, sizeof(mstruct));
		  
		  /* termino */
		  exit(bufferchild.c1);
	          break;

	       case -1:
	          zprintf(2, "\t[%d, figlio] Errore : fork() fallita\n\n", getpid());
		  exit(5);
		  break;
	    }
            
            /* azioni del figlio */
	    /* chiudo i lati della pipe inutilizzati */
            close(p2[1]);

	    read(p2[0], &bufferchild, sizeof(mstruct));
	    write(p[i][1], &bufferchild, sizeof(mstruct));

	    /* attendo la terminazione del nipote */
            if (wait(&status) < 0)
	    {
	       zprintf(2, "\t[%d, figlio] Errore : wait() fallita\n\n", getpid());
	       exit(6);
	    }

            /* termino */
	    exit(WEXITSTATUS(status));
	    break;

	 /* caso di fork() fallita */
	 case -1:
	    zprintf(2, "\t[%d, padre] Errore : fork() fallita\n\n", getpid());
	    exit(7);
	    break;
      }
   }

   /* codice del padre */
   /* chiudo i lati della pipe inutili per il padre */
   for (i = 0; i < child_n; i++)
   {
      close(p[i][1]);
   }

   zprintf(1, "\n");
   /* azioni del padre */
   for (i = 0; i < child_n; i++)
   {
      read(p[i][0], &bufferfather, sizeof(mstruct));
      zprintf(1, "[%d, padre] ricevuto : ", getpid());
      printstruct(bufferfather);
      zprintf(1, "\n");
   }

   zprintf(1, "\n\n[%d, padre] Elaborazione terminata\n\n", getpid());

   /* attendo la terminazione dei processi figli e ne stampo il valore di ritorno */
   for (i = 0; i < child_n; i++)
   {
      if ((pid = wait(&status)) < 0)
      {
         zprintf(2, "\t[%d, padre] Errore : wait() fallita\n\n");
	 exit(8);
      }      

      zprintf(1, "[%d, padre] il figlio di pid %d è terminato con codice %d\n", getpid(), pid, WEXITSTATUS(status));
   }
   exit(EXIT_SUCCESS);
}

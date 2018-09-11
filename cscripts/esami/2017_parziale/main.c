

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
}mstruct;

void printstruct(mstruct s)
{
   zprintf(1, " c1 = %ld, c2 = %d, c3 = %ld ", s.c1, s.c2, s.c3);
}

/* tipo di dato apposito per semplificare l'utilizzo delle pipe multiple */
typedef int pipe_t[2];

/* main : funzione contenente il codice del processo padre */
int main(int argc, char **argv) 
{
   /* variabili di iterazione dei cicli */
   int i, j;

   /* buffer di lettura e scrittura */
   mstruct bufferchild, bufferfather;

   int status; /* gestione del valore di uscita dei processi figli*/
   pid_t pid; /* [GENERIC] variabile/array per gestire il/i pid creato/i tramite fork() */
   pid_t best_pid; /* pid del processo che ha calcolato il numero maggiore di occorrenze */
   int child_n; /* variabili atte a contenere il numero di processi figli e nipoti da generare */
   pipe_t *p; /* variabile puntatore alla zona di memoria contenente tutte le pipe in utilizzo */
   char cx; /* carattere passato dall'utente allo script */
   int fd; /* variabile di gestione di un singolo file descriptor */
   char buffer; /* buffer di lettura per singolo carattere */
   long int found; /* contatore per numero di occorrenze di cx all'interno del file correntemente analizzato da un processo figlio */

   int min_argc = 4; /* valore minimo di argc */
   
   /* controllo che il numero dei parametri passati allo script sia appropriato */
   if (argc < min_argc) 
   {
      /* stampo tutti gli errori sul file avente file descriptor 2, ovvero su stderror */
      zprintf(2, "\t[%d, padre] Utilizzo : %s file_1 file_2 [file_3] ... [file_n] carattere\n\n", getpid(), argv[0]);
      exit(1);
   }
   
   /* stabilisco il numero di processi da creare (tanti quanti i file passati) */
   child_n = argc - 2;

   /* [GENERIC] controllo i parametri passati a linea di comando */
   if (child_n <= 0)
   {
      zprintf(2, "\t[%d, padre] Errore : il numero dei processi figli deve essere positivo\n\n", getpid());
      exit(2);
   }

   /* controllo che l'ultimo parametro sia un singolo carattere */
   if (argv[argc - 1][1] != 0)
   {
      zprintf(2, "\t[%d, padre] Errore : l'ultimo parametro deve essere un singolo carattere\n\n", getpid());
      exit(3);
   }
   cx = argv[argc - 1][0];

   /* alloco la zona di memoria dedicata alle pipe tra processo padre e processi figli, con controllo dell'errore */
   p = (pipe_t *) malloc(sizeof(pipe_t) * child_n);
   if (p == NULL) 
   {
      zprintf(2, "\t[%d, padre] Errore : malloc() fallita \n\n", getpid());
      exit(4);
   }
   
   /* inizializzo le pipeline allocate attraverso la primitiva pipe() */
   for (i = 0; i < child_n; i++) 
   { 
      if (pipe(p[i]) < 0) 
      { 
         zprintf(2, "\t[%d, padre] Errore : pipe() fallita \n\n");
         exit(5);
      }
   }

   /* creo i processi figli */
   for (i = 0; i < child_n; i++)
   {
      /* apro il file i-esimo, controllando l'esito dell'operazione. Ciò mi consente di uscire dal processo padre in caso di errore e, allo stesso tempo, di far ereditare al processo figlio il file descriptor corrispondente al file che deve elaborare */
      if ((fd = open(argv[i + 1], O_RDONLY)) < 0)
      {
         zprintf(2, "\t[%d, padre] Errore : open() fallita sul file %s\n\n", getpid(), argv[i + 1]);
	 exit(6);
      }

      /* controllo l'esito della fork() */
      switch(fork())
      {
         case 0:
	    /* codice del figlio */
	    
            /* chiudo i lati della pipe non utilizzati dal figlio */
	    for (j = 0; j < child_n; j++)
	    {
	       if (j != i) close(p[j][1]);
	       if (j != i - 1) close(p[j][0]);
	    }

            /* azioni del figlio */
	    /* ricevo, dal figlio precedente, la struttura dati, ma solo se non mi trovo nel primo processo; nel caso, inizializzo la struttura */
	    if (i == 0)
	    {
	       bufferchild.c1 = 0;
	       bufferchild.c3 = 0;
	    }
	    else
            {
	       if(read(p[i - 1][0], &bufferchild, sizeof(mstruct)) != sizeof(mstruct))
	       {
	          zprintf(2, "\t[%d, figlio] Errore : read() fallita\n\n", getpid());
		  exit(7);
	       }
	    }

            /* leggo byte per byte il file associato al processo in cui mi trovo, controllando ad ogni iterazione se il carattere (byte) letto corrisponde a quello cercato */
	    found = 0;
	    while(read(fd, &buffer, sizeof(char)) == sizeof(char))
	       if (buffer == cx) found++;

	    /* se il numero di occorrenze trovate supera quelle massime calcolate in precedenza, aggiorno la struttura ricevuta (o inizializzata, nel caso del processo P0) */
	    if (found > bufferchild.c1)
	    {
	       bufferchild.c1 = found;
	       bufferchild.c2 = i;
	    }

	    /* in ogni caso, l'accumulatore delle occorrenze trovate deve essere aggiornato */
	    bufferchild.c3 += found;

	    /* spedisco la struttura propriamente elaborata al processo figlio successivo */
	    if (write(p[i][1], &bufferchild, sizeof(mstruct)) != sizeof(mstruct))
	    {
	       zprintf(2, "\t[%d, figlio] Errore : write() fallita\n\n", getpid());
	       exit(8);
	    }
            
            /* chiudo il file corrispondente */
	    close(fd);

            /* termino ritornando, come richiesto, l'indice d'ordine */
	    exit(i);
	    break;

	 /* caso di fork() fallita */
	 case -1:
	    zprintf(2, "\t[%d, padre] Errore : fork() fallita\n\n", getpid());
	    exit(9);
	    break;
      }

      /* chiudo il file anche all'interno del processo padre */
      close(fd);
   }

   /* codice del padre */
   /* chiudo i lati della pipe inutili per il padre */
   for (i = 0; i < child_n - 1; i++)
   {
      close(p[i][0]);
      close(p[i][1]);
   }
   close(p[child_n - 1][1]);

   /* azioni del padre */
   /* ricevo, dall'ultimo processo (quindi attraverso l'ultima pipe) la struttura elaborata da ogni processo */
   if (read(p[child_n - 1][0], &bufferfather, sizeof(mstruct)) != sizeof(mstruct))
   {
      zprintf(2, "\t[%d, padre] Errore : read() fallita\n\n", getpid());
      exit(10);
   }
   zprintf(1, "\n");
   
   /* attendo la terminazione dei processi figli */
   for (i = 0; i < child_n; i++)
   {
      if ((pid = wait(&status)) < 0)
      {
         zprintf(2, "\t[%d, padre] Errore : wait() fallita\n\n");
	 exit(11);
      }      
      zprintf(1, "\n[%d, padre] Processo di pid %d terminato con codice %d\n", getpid(), pid, WEXITSTATUS(status));

      if (WEXITSTATUS(status) == bufferfather.c2)
         best_pid = pid;
   }
   zprintf(1, "\n\n[%d, padre] Esecuzione terminata\n\nSono state trovate, in totale, %d occorrenze del carattere %c.\nIl file che ne contiene di più (%d) è %s, associato al processo di pid %d\n\n", getpid(), bufferfather.c3, cx, bufferfather.c1, argv[bufferfather.c2 + 1], best_pid);
  exit(EXIT_SUCCESS); 
}

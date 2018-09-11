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
   char chr;
   long int occ;
   char proc;
   int pid;
}mstruct;

/* funzione di stampa di una struttura dati */
void printstruct(mstruct s)
{
   zprintf(1, " chr = %c, occ = %ld, proc = %c, pid = %d ", s.chr, s.occ, s.proc, s.pid);
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
   pipe_t p2nf; /* singola pipe di comunicazione nipote --> figlio */
   pipe_t p2fn; /* singola pipe di comunicazione figlio --> nipote */
   int min_argc = 5; /* valore minimo di argc */
   int fd; /* gestore di singolo file descriptor */
   char AM; /* carattere maiuscolo trovato */
   long int Nocc, Pocc;  /* occorrenze di AM nel file associato, rispettivamente, a figlio o nipote */
   char cbuff; /* buffer di lettura di un singolo carattere */

   /* controllo che il numero dei parametri passati allo script sia appropriato */
   if (argc < min_argc || (argc - 1) % 2 != 0) 
   {
      /* stampo tutti gli errori sul file avente file descriptor 2, ovvero su stderror */
      zprintf(2, "\t[%d, padre] Utilizzo : %s file_1 file_2 [file_3] ... [file_n] file_n+1 file_n+2 [file_n+3] ... [file_2n]\n\n", getpid(), argv[0]);
      exit(1);
   }
   
   /* stabilisco il numero di processi da creare (tanti quanti i file passati) */
   child_n = (argc - 1)/2;

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
      if ((fd = open(argv[i + 1], O_RDONLY)) < 0)
      {
         zprintf(2, "\t[%d, padre] errore : open() fallita sul file %s\n\n", getpid(), argv[i + 1]);
	 exit(4);
      }

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
	    if (pipe(p2fn) < 0 || pipe(p2nf) < 0)
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
                  close(p2nf[0]);
		  close(p2fn[1]);

		  /* azioni del nipote */
		  /* attendo che arrivi il carattere maiuscolo dal figlio */
		  read(p2fn[0], &AM, sizeof(char));

		  zprintf(1, "[%d, nipote] Carattere arrivato : %c\n", getpid(), AM);

		  /* termino */
		  exit(0);
	          break;

	       case -1:
	          zprintf(2, "\t[%d, figlio] Errore : fork() fallita\n\n", getpid());
		  exit(5);
		  break;
	    }
            
	    /* chiudo i lati della pipe inutilizzati */
            close(p2nf[1]);
	    close(p2fn[0]);

            /* azioni del figlio */
	    /* comincio a scorrere il file associato al processo */
	    /* inizialmente, considero AM come non impostato (a valore -1) */
	    AM = -1;
	    Pocc = 0;
	    while (read(fd, &cbuff, sizeof(char)) > 0)
	    {
	       /* imposto il carattere AM come il primo carattere maiuscolo che trovo */
               if (AM == -1 && (cbuff >= 'a' && cbuff <= 'z'))
	       {
		  /* memorizzo il carattere, lo invio al nipote e riavvolgo il file per tenere conto anche dei caratteri precedenti AM */
	          AM = cbuff;
		  write(p2fn[1], &AM, sizeof(char));
		  lseek(fd, 0, SEEK_SET);
	       }

	       if (cbuff == AM) Pocc++;
	    }

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

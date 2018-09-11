/* inclusione delle librerie */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

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

/* tipo di dato apposito per semplificare l'utilizzo delle pipe multiple */
typedef int pipe_t[2];

/* main : funzione contenente il codice del processo padre */
int main(int argc, char **argv) 
{
   /* variabili locali */

   int i, j; /* variabili di iterazione dei cicli */
   int status; /* gestione del valore di uscita dei processi figli*/
   pid_t pid; /* variabile di gestione di un singolo pid */
   pid_t *pids; /* zona di memoria contenente i pid dei processi creati */
   int child_n; /* variabili atte a contenere il numero di processi figli e nipoti da generare */
   pipe_t *p; /* pipe di comunicazione figlio --> padre */
   pipe_t *p2; /* pipe di comunicazione padre --> figlio */
   int fd_merge; /* file descriptor del file Merge creato dal processo padre */
   char buf, cbuf; /* buffer di lettura di un singolo carattere */
   int fd; /* variabile di gestione di un singolo file */
   char garbage;

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

   /* controllo i parametri passati a linea di comando */
   if (child_n <= 0)
   {
      zprintf(2, "\t[%d, padre] Errore : il numero dei processi figli deve essere positivo\n\n", getpid());
      exit(2);
   }

   /* alloco la zona di memoria dedicata alle pipe tra processo padre e processi figli (e viceversa), con controllo dell'errore */
   p = (pipe_t *) malloc(sizeof(pipe_t) * child_n);
   p2 = (pipe_t *) malloc(sizeof(pipe_t) * child_n);
   pids = (pid_t *) malloc(sizeof(pid_t) * child_n);
   if (p == NULL || p2 == NULL || pids == NULL) 
   {
      zprintf(2, "\t[%d, padre] Errore : malloc() fallita \n\n", getpid());
      exit(3);
   }
   
   /* inizializzo le pipeline allocate attraverso la primitiva pipe() */
   for (i = 0; i < child_n; i++) 
   { 
      if (pipe(p[i]) < 0 || pipe(p2[i]) < 0) 
      { 
         zprintf(2, "\t[%d, padre] Errore : pipe() fallita \n\n");
         exit(4);
      }
   }

   /* creo il file Merge, con controllo dell'errore */
   if ((fd_merge = creat("Merge", 0644)) < 0) 
   {
      zprintf(2, "\t[%d, padre] Errore : creat() fallita\n\n");
      exit(5);
   }

   /* creo i processi figli */
   for (i = 0; i < child_n; i++)
   {
      /* controllo l'esito della fork() */
      switch(pids[i] = fork())
      {
         case 0:
	    /* codice del figlio */
	    
            /* chiudo i lati della pipe non utilizzati dal figlio */
	    for (j = 0; j < child_n; j++)
	    {
	       close(p[j][0]);
	       close(p2[j][1]);
	       if (j != i) 
	       {
	          close(p[j][1]);
		  close(p2[j][0]);
	       }
	    }

            /* azioni del figlio */
            /* apro il file associato al processo corrente, con controllo dell'errore */
            if ((fd = open(argv[i + 1], O_RDONLY)) < 0)
	    {
	       zprintf(2, "\t[%d, figlio] Errore : open() fallita sul file %s\n\n", getpid(), argv[i + 1]);
	       exit(6);
	    }
            
	    /* comincio a leggere il file associato al processo byte per byte */
	    char dio;
	    while(read(fd, &cbuf, sizeof(char)) > 0)
	    {
	       /* aspetto il segnale di sincronizzazione dal padre */
	       read(p2[i][0], &dio, sizeof(char));
	       /* invio al padre il carattere letto */
	       write(p[i][1], &cbuf, sizeof(char));
	    }

            /* termino */
	    exit(EXIT_SUCCESS);
	    break;

	 /* caso di fork() fallita */
	 case -1:
	    zprintf(2, "\t[%d, padre] Errore : fork() fallita\n\n", getpid());
	    exit(5);
	    break;
      }
   }

   /* codice del padre */
   /* chiudo i lati della pipe inutili per il padre */
   for (i = 0; i < child_n; i++)
   {
      close(p[i][1]);
      close(p2[i][0]);
   }

   /* azioni del padre */
   zprintf(1, "\n");
   i = 0;
   /* ciclo infinito che deve terminare solo quando un figlio arriva al termine del suo file */
   int termina = garbage = 0;
   while(1)
   {
      /* sblocco l'i-esimo figlio e leggo il carattere da esso letto */
      for (i = 0; i < child_n; i++)
      {
         zprintf(1, "scrivo...\n");
         write(p2[i][1], &garbage, sizeof(char));
         if (read(p[i][0], &cbuf, sizeof(char)) != sizeof(char))
	 {
	    termina = 1;
	    break;
	 }
	 zprintf(1, "ricevo %c\n", cbuf);
         /* scrivo il carattere su Merge */
         //write(fd_merge, &buf, sizeof(char);)
      }
      if (termina) break;
   }

   /* termino tutti i processi tranne l'i-esimo */
   for (j = 0; j < child_n; j++)
      if (j != i)
         kill(pids[j], SIGKILL);

   zprintf(1, "\n\n[%d, padre] Elaborazione terminata\n\n", getpid());
   /* attendo la terminazione dei processi figli */
   for (i = 0; i < child_n; i++)
   {
      if ((pid = wait(&status)) < 0)
      {
         zprintf(2, "\t[%d, padre] Errore : wait() fallita\n\n");
	 exit(6);
      }      
      zprintf(1, "[%d, padre] Il processo con pid %d è terminato con codice %d\n", getpid(), pid, WEXITSTATUS(status));
   }

   /* chiudo il file Merge */
   close(fd_merge);
   exit(EXIT_SUCCESS);
}

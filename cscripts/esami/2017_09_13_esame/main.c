
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

/* tipo di dato apposito per semplificare l'utilizzo delle pipe multiple */
typedef int pipe_t[2];

/* main : funzione contenente il codice del processo padre */
int main(int argc, char **argv) 
{
   /* variabili di iterazione dei cicli */
   int i, j;

   int status; /* gestione del valore di uscita dei processi figli*/
   pid_t pid; /* [GENERIC] variabile/array per gestire il/i pid creato/i tramite fork() */
   int child_n; /* variabili atte a contenere il numero di processi figli e nipoti da generare */
   pipe_t *p; /* variabile puntatore alla zona di memoria contenente tutte le pipe in utilizzo */
   int min_argc = 3; /* valore minimo di argc */
   char rowbuffer[250]; /* buffer di riga */
   int fd_target, fd; /* variabili per gestire, rispettivamente, i file descriptors inerenti il file su cui scrivere e i file da cui leggere */
   char childbuffer; /* buffer di lettura di un singolo carattere per il figlio */
   int curlen = 0;
   
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
      exit(3);
   }
   
   /* inizializzo le pipeline allocate attraverso la primitiva pipe() */
   for (i = 0; i < child_n; i++) 
   { 
      if (pipe(p[i]) < 0) 
      { 
         zprintf(2, "\t[%d, padre] Errore : pipe() fallita \n\n");
         exit(4);
      }
   }

   /* creo il file Ultime_Linee */
   if ((fd_target = creat("Ultime_Linee", 0644)) < 0)
   {
      zprintf(2, "\t[%d, padre] Errore : creat() fallita\n\n", getpid());
      exit(5);
   }

   /* inizializzo il buffer di riga con soli terminatori di stringa C */
   memset(rowbuffer, 0, sizeof(rowbuffer));

   /* creo i processi figli */
   for (i = 0; i < child_n; i++)
   {
      /* apro il file associato al processo i-esimo, con controllo dell'errore */	   
      if ((fd = open(argv[i + 1], O_RDONLY)) < 0)
      {
         zprintf(2, "\t[%d, padre] Errore : open() sul file %s fallita\n\n", getpid(), argv[i + 1]);         exit(6);		 
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

            /* azioni del figlio */
	    /* leggo il file carattere per carattere... */
	    j = 0;
            while (read(fd, &childbuffer, sizeof(char)) == sizeof(char))
	    {
	       /* memorizzo il carattere solo se sono entro i limiti di grandezza del buffer */
	       if (j < 250)
	          rowbuffer[j++] = childbuffer;

               /* se incontro il terminatore di linea, memorizzo la lunghezza di quella appena letta e passo alla prossima. Se questa è l'ultima linea, il rowbuffer non verrà sovrascritto ulteriormente conterrà, per l'appunto, l'ultima linea */
	       if (childbuffer == '\n')
	       {
		  curlen = j;
	          j = 0;
	       }
	    }

	    /* posiziono il terminatore di stringa C alla fine della stringa e la invio al padre */
            write(p[i][1], rowbuffer, sizeof(rowbuffer));

	    /* termino */
	    exit(curlen);
	    break;

	 /* caso di fork() fallita */
	 case -1:
	    zprintf(2, "\t[%d, padre] Errore : fork() fallita\n\n", getpid());
	    exit(5);
	    break;
      }

      /* chiudo il file associato al processo corrente */
      close(fd);
   }

   /* codice del padre */
   /* chiudo i lati della pipe inutili per il padre */
   for (i = 0; i < child_n; i++)
   {
      close(p[i][1]);
   }

   /* azioni del padre */
   for (i = 0; i < child_n; i++)
   {
      /* leggo tutta la stringa C in arrivo dal figlio */
      read(p[i][0], rowbuffer, sizeof(rowbuffer));

      /* scorro la stringa carattere per carattere fino al terminatore di linea, scrivendo mano a mano ogni carattere sul file creato */
      for (j = 0; rowbuffer[j] != '\n'; j++)
         write(fd_target, &rowbuffer[j], sizeof(char));
      zprintf(1, "\n");
   }

   zprintf(1, "[%d, padre] Elaborazione terminata\n\n", getpid());
   /* attendo la terminazione dei processi figli */
   for (i = 0; i < child_n; i++)
   {
      if ((pid = wait(&status)) < 0)
      {
         zprintf(2, "\t[%d, padre] Errore : wait() fallita\n\n");
	 exit(6);
      }      
      
      zprintf(1, "[%d, padre] Il processo di pid %d ha ritornato il valore %d\n", getpid(), pid, WEXITSTATUS(status));
   }

   /* chiudo il file creato e termino */
   close(fd_target);
   exit(EXIT_SUCCESS); 
}

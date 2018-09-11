/* inclusione delle librerie */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

/* zprintf : funzione che simula il comportamento della fprintf attraverso chiamate di sistema. E' sa */
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
   char result[4096];
}mstruct;

/* funzione di stampa di una singola struttura dati */
void printstruct(mstruct s)
{
   zprintf(1, " output del comando \n%s", s.result);
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
   pid_t pid; /* variabile/array per gestire il/i pid creato/i tramite fork() */
   int child_n; /* variabili atte a contenere il numero di processi figli e nipoti da generare */
   pipe_t *p; /* variabile puntatore alla zona di memoria contenente tutte le pipe in utilizzo */
   pipe_t p2; /* pipe di comunicazione tra figlio e nipote */
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
      switch (fork())
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
		  /* chiudo lo standard output del nipote così da ridirigerlo sulla pipe di comunicazione con il figlio che l'ha generato */
		  close(1);
		  dup(p2[1]);
                  /*close(0);
		  if (open("provafile.txt", O_RDONLY) < 0)
		  {
		     zprintf(2, "\t[%d, nipote] Errore : open() fallita\n\n", getpid());
		     exit(5);
		  }*/

                  /* eseguo il comando richiesto */
		  execlp("ls", "ls", "-l", (char *)0);

		  /* la primitiva exec() ritorna solo in caso di errore, quindi esco dal processo figlio di conseguenza */
		  exit(EXIT_FAILURE);
	          break;

	       case -1:
	          zprintf(2, "\t[%d, figlio] Errore : fork() fallita\n\n", getpid());
		  exit(6);
		  break;
	    }
            
            /* azioni del figlio */
	    /* chiudo i lati della pipe inutilizzati */
            close(p2[1]);

	    /* lettura, carattere per carattere, dell'output del comando */
	    j = 0;
            while(read(p2[0], &bufferchild.result[j++], sizeof(char)) == sizeof(char));

	    if (write(p[i][1], bufferchild.result, 4096) != 4096)
	    {
	       zprintf(2, "\t[%d, figlio] Errore : write() fallita\n\n", getpid());
	       exit(7);
	    }

	    /* attendo la terminazione del nipote */
            if (wait(&status) < 0)
	    {
	       zprintf(2, "\t[%d, figlio] Errore : wait() fallita\n\n", getpid());
	       exit(8);
	    }

	    /* controllo che la exec del nipote sia andata a buon fine */
            if (WEXITSTATUS(status) == EXIT_FAILURE)
	    {
	       zprintf(2, "\t[%d, figlio] Errore : exec() fallita\n\n", getpid());
	       exit(9);
	    }

            /* termino */
	    exit(EXIT_SUCCESS);
	    break;

	 /* caso di fork() fallita */
	 case -1:
	    zprintf(2, "\t[%d, padre] Errore : fork() fallita\n\n", getpid());
	    exit(10);
	    break;
      }
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
      if (read(p[i][0], bufferfather.result, 4096) != 4096)
      {
         zprintf(2, "\t[%d, padre] Errore : read() fallita\n\n", getpid());
	 exit(11);
      }
      zprintf(1, "[%d, padre] ricevuto : \n", getpid());
      printstruct(bufferfather);
      zprintf(1, "\n");
   }

   /* attendo la terminazione dei processi figli */
   for (i = 0; i < child_n; i++)
   {
      if ((pid = wait(&status)) < 0)
      {
         zprintf(2, "\t[%d, padre] Errore : wait() fallita\n\n");
	 exit(12);
      }      
   }
   exit(EXIT_SUCCESS); 
}

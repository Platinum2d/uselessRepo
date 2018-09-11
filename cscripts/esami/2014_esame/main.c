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

   /* buffer di lettura e scrittura */
   char bufferchild[20], buffergrandson[20], bufferchar;
   int conversion, bufferfather;

   long int sum = 0; /* variabile per contenere la somma finale */
   int status; /* gestione del valore di uscita dei processi figli*/
   pid_t pid; /* [GENERIC] variabile/array per gestire il/i pid creato/i tramite fork() */
   int child_n; /* variabili atte a contenere il numero di processi figli e nipoti da generare */
   pipe_t *p; /* variabile puntatore alla zona di memoria contenente tutte le pipe in utilizzo */
   pipe_t wcp; /* pipe per scambiare i risultati delle wc dal nipote al figlio */

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

   /* [GENERIC] controllo i parametri passati a linea di comando */
   if (child_n <= 0)
   {
      zprintf(2, "\t[%d, padre] Errore : il numero dei processi figli deve essere positivo\n\n", getpid());
      exit(2);
   }

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
            
	    /* creo la pipe per il nipote */
	    pipe(wcp);

	    /* creo il nipote */
	    switch(fork())
	    {
	       /* codice del nipote */
	       case 0:
		  /* chiudo tutti i lati della pipe non usati nel nipote per comunicare con il figlio */
		  close(wcp[0]);

                  /* ridirigo standard input e standard output */
		  close(0);
		  if (open(argv[i + 1], O_RDONLY) < 0)
		  {
		     zprintf(2, "\t[%d, nipote] Errore : open() fallita\n\n", getpid());
		     exit(5);
		  }
		  close(1);
		  dup(wcp[1]);
		  close(wcp[1]);
                  
		  /* scrivo al processo figlio il risultato della wc tramite la ridirezione */
                  execlp("wc", "wc", "-l", (char *)0);

		  /* termino */
		  exit(EXIT_FAILURE);
		  break;

	       case -1:
		  zprintf(2, "\t[%d, figlio] Errore : fork() fallita\n\n", getpid());
		  exit(6);
		  break;
	    }

	    /* chiudo le porte della pipe non usate tra figlio e nipote */
	    close(wcp[1]);

	    /* leggo il risultato del nipote */
	    j = 0;
	    while (read(wcp[0], &bufferchar, sizeof(bufferchar) > 0))
	    {
	       bufferchild[j] = bufferchar;
	       j++;
	    }

	    /* converto la stringa in intero */
            conversion = atoi(bufferchild);

	    /* aspetto che il nipote finisca */
	    if (wait(&status) < 0)
	    {
	       zprintf(2, "\t[%d, figlio] Errore : wait() fallita\n\n", getpid());
	       exit(8);
	    }

	    /* scrivo al padre l'intero letto dal nipote */
	    if (write(p[i][1], &conversion, sizeof(conversion)) != sizeof(conversion))
	    {
	       zprintf(2, "\t[%d, figlio] Errore : write() fallita\n\n", getpid());
	       exit(9);
	    }

            /* termino */
	    exit(WEXITSTATUS(status));
	    break;

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

   /* leggo tutti i risultati dei processi figli e li sommo man mano ad ogni iterazione */
   for (i = 0; i < child_n; i++)
   {
      read(p[i][0], &bufferfather, sizeof(bufferfather));
      sum += bufferfather;
   }

   /* attendo la terminazione dei processi figli */
   for (i = 0; i < child_n; i++)
   {
	   pid = wait(&status);
	   zprintf(1, "[%d, padre] il processo con pid %d è terminato con codice %d\n", getpid(), pid, WEXITSTATUS(status));
   }

   zprintf(1, "\n\n[%d, padre] la somma delle linee dei file è pari a %ld\n\n", getpid(), sum);
   
  exit(EXIT_SUCCESS); 
}

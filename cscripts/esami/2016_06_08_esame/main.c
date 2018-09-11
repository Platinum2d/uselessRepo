/* inclusione delle librerie */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <time.h>
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

/* funzione di generazione di un numero random tra 0 e n - 1 */
int mia_random(int n)
{
   int casuale;
   casuale = rand() % n;
   return casuale;
}

/* tipo di dato apposito per semplificare l'utilizzo delle pipe multiple */
typedef int pipe_t[2];

/* main : funzione contenente il codice del processo padre */
int main(int argc, char **argv) 
{
   /* variabili locali */

   int i, j; /* variabili di iterazione dei cicli */
   int bufferchild, bufferfather; /* buffer di lettura e scrittura */
   int status; /* gestione del valore di uscita dei processi figli*/
   pid_t pid; /* variabile di gestione di un singolo pid */
   int child_n; /* variabili atte a contenere il numero di processi figli e nipoti da generare */
   pipe_t *p; /* variabile puntatore alla zona di memoria contenente le pipe per la comunicazione figlio --> padre */
   pipe_t *p2; /* variabile puntatore alla zona di memoria contenente le pipe per la comunicazione padre --> figlio */
   int H; /* numero di linee di ogni file passato come parametro allo script */
   int fd_target; /* file descriptor del file temporaneo */
   int fd; /* singolo file descriptor */
   int min_argc = 6; /* valore minimo di argc */
   int rowcount; /* contatore di righe trovate all'interno di ogni processo */
   int rowlen; /* contatore dei caratteri all'interno di una singola riga */
   int random; /* numero generato a caso dal padre */
   int randomline; /* linea randomica */
   char randomchar; /* carattere di posizione randomica prelevato all'interno della linea randomica */
   char childbuff; /* buffer di lettura del singolo carattere per il figlio */
   char *currentline; /* linea correntemente analizzata da ogni processo */
   int written; /* numero di caratteri scritti da ogni processo sul file */
   
   /* controllo che il numero dei parametri passati allo script sia appropriato */
   if (argc < min_argc) 
   {
      /* stampo tutti gli errori sul file avente file descriptor 2, ovvero su stderror */
      zprintf(2, "\t[%d, padre] Utilizzo : %s file_1 file_2 file_3 file_4 [file_5] ... [file_n] H\n\n", getpid(), argv[0]);
      exit(1);
   }
   
   /* stabilisco il numero di processi da creare (tanti quanti i file passati) */
   child_n = argc - 2;
   
   /* controllo che H sia strettamente positivo e minore di 255 */
   H = atoi(argv[argc - 1]);
   if (H <= 0 || H >= 255)
   {
      zprintf(2, "\t[%d, padre] Errore : H deve essere strettamente compreso tra 0 e 255\n\n");
      exit(2);
   }

   /* alloco la zona di memoria dedicata alle pipe tra processo padre e processi figli, con controllo dell'errore */
   p = (pipe_t *) malloc(sizeof(pipe_t) * child_n);
   p2 = (pipe_t *) malloc(sizeof(pipe_t) * child_n);
   if (p == NULL) 
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

   /* creo il file temporaneo */
   if ((fd_target = creat("/tmp/creato", 0644)) < 0)
   {
      zprintf(2, "\t[%d, padre] Errore : creat() fallita\n\n");
      exit(5);
   }

   /* inizializzo il seme per la generazione di numeri casuali */
   srand(time(NULL));

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
	       close(p2[j][1]);
	       if (j != i) 
	       {
	          close(p[j][1]);
		  close(p2[j][0]);
	       }
	    }

            /* azioni del figlio */
            /* provo ad aprire il file i-esimo, associato al processo corrente */
            if ((fd = open(argv[i + 1], O_RDONLY)) < 0)
	    {
	       zprintf(2, "\t[%d, figlio] Errore : open() fallita sul file %s\n\n", getpid(), argv[i + 1]);
	       exit(5);
	    }

	    /* leggo il file finchè il numero di linee trovate non è pari ad H */
	    rowcount = 0;
	    written = 0;
            while (rowcount < H)
	    {
	       rowlen = 0;
	       /* conto i caratteri all'interno della linea corrente */
	       do
	       {
	          read(fd, &childbuff, sizeof(char));
		  currentline = realloc(currentline, (++rowlen) * sizeof(char));
		  currentline[rowlen - 1] = childbuff;
	       }
	       while(childbuff != '\n');
	       /* invio al padre la lunghezza e aumento il numero di righe analizzate */
	       write(p[i][1], &rowlen, sizeof(int));
	       rowcount++;

	       /* ricevo l'indice creato dal padre e controllo che non sia superiore la lunghezza di riga corrente */
	       read(p2[i][0], &bufferchild, sizeof(int));
	       if (bufferchild < rowlen)
	       {
	          write(fd_target, &currentline[bufferchild], sizeof(char));
		  written++;
	       }
	    }

	    /* chiudo il file aperto nel processo corrente */
	    close(fd);
            /* termino */
	    exit(written);
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
   /* leggo per H volte dai processi figli, ovvero tante volte quante sono le linee di tutti i file */
   for (j = 0; j < H; j++)
   {
      /* stabilisco randomicamente quale lunghezza considerare */
      random = mia_random(child_n);
      for (i = 0; i < child_n; i++)
      {
	 /* leggo la lunghezza scritta dal processo i-esimo */
	 read(p[i][0], &bufferchild, sizeof(int));
         if (i == random) randomline = bufferchild;
      }
      /* individuo un altro random, stavolta come indice all'interno della linea considerata */
      random = mia_random(randomline);

      /* invio l'indice generato ai figli */
      for (i = 0; i < child_n; i++)
      {
         write(p2[i][1], &random, sizeof(int));
      }
   }

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

   /* chiudo il file temporaneo e termino */
   close(fd_target);
   exit(EXIT_SUCCESS); 
}

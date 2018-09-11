

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
}mstruct;

void printstruct(mstruct s)
{
   zprintf(1, " c1 = %ld, c2 = %d", s.c1, s.c2);
}

/* tipo di dato apposito per semplificare l'utilizzo delle pipe multiple */
typedef int pipe_t[2];

int getlengthof(char *filepath)
{
   int fd, count = 0;
   char buf; 

   if ((fd = open(filepath, O_RDONLY)) < 0)
   {
      zprintf(2, "\t[%d, padre] Errore : open() fallita sul file %s\n\n", getpid(), filepath);
      exit(5);
   }

   while(read(fd, &buf, sizeof(char))) count++;

   return count;
}

/* main : funzione contenente il codice del processo padre */
int main(int argc, char **argv) 
{
   /* variabili di iterazione dei cicli */
   int i, j;

   /* buffer di lettura e scrittura */
   mstruct bufferchild, bufferfather;

   int status; /* gestione del valore di uscita dei processi figli*/
   pid_t pid; /* [GENERIC] variabile/array per gestire il/i pid creato/i tramite fork() */
   int child_n; /* variabili atte a contenere il numero di processi figli e nipoti da generare */
   pipe_t *p; /* variabile puntatore alla zona di memoria contenente tutte le pipe in utilizzo */
   int min_argc = 3; /* valore minimo di argc */
   int fd; /* singolo file descriptor */
   char cbuff; /* buffer di lettura dal i-esimo file */
   int ibuff; /* buffer di conversione di un singolo carattere letto dal i-esimo file */
   int len; /* lunghezza di ogni file, utile al ciclo for del padre */   
   char lastchar;

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

   /* ricavo la lunghezza dei file, andando a calcolare quella del primo */
   len = getlengthof(argv[1]);

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
               if (j != i) close(p[j][1]);
               if (j != i + 1) close(p[j][0]);
	    }

            /* azioni del figlio */
            /* comincio a leggere ogni carattere del file i-esimo, con controllo di apertura */
            if ((fd = open(argv[i + 1], O_RDONLY)) < 0)
            {
               zprintf(2, "\n[%d, figlio] Errore : open() fallita sul file %s\n\n",  getpid(), argv[i + 1]);
               exit(6);
            }
            
            while(read(fd, &cbuff, sizeof(char)))
            {
               /* converto il carattere letto sfruttando la sua codifica ASCII */
               ibuff = (int)(cbuff - '0');

               /* leggo la struttura in arrivo dal processo precedente */
               if (i != child_n - 1)
               {
                  /* se il processo precedente è arrivato alla fine del file, termino il corrente */
                  if (read(p[i + 1][0], &bufferchild, sizeof(mstruct)) != sizeof(mstruct))
                     break;
               }              
               else
               {
		  /* se mi trovo nell'ultimo figlio, imposto la struttura con dei valori di partenza che siano validi per la metodologia di comunicazione specificata */
                  bufferchild.c1 = getpid();
                  bufferchild.c2 = ibuff;
               }

               /* imposto la struttura dati e la spedisco al processo i-1esimo */
               if (ibuff > bufferchild.c2)
               {
		  /* ho estratto dal file corrente un numero maggiore di quello massimo precedentemente calcolato: modifico la struttura letta con i dati del processo corrente */
                  bufferchild.c1 = getpid();
                  bufferchild.c2 = ibuff;
                  lastchar = cbuff;
               }
               write(p[i][1], &bufferchild, sizeof(mstruct));
            } 

	    /* chiudo il file precedentemente aperto con successo */
            close(fd);

            /* termino */
	    exit(lastchar);
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
   close(p[0][1]);
   for (i = 1; i < child_n; i++)
   {
      close(p[i][0]);
      close(p[i][1]);
   }

   /* azioni del padre */
   for (i = 0; i < len - 1; i++)
   {
      /* leggo la struttura in arrivo dal figlio P0 */
      mstruct s;
      read(p[0][0], &s, sizeof(mstruct));
      zprintf(1, "[%d, padre] Ricevuta struttura: ");
      printstruct(s);
      zprintf(1, "\n");
   }

   /* attendo la terminazione dei processi figli */
   zprintf(1, "\n\n");
   for (i = 0; i < child_n; i++)
   {
      if ((pid = wait(&status)) < 0)
      {
         zprintf(2, "\t[%d, padre] Errore : wait() fallita\n\n");
	 exit(8);
      }      
      zprintf(1, "[%d, padre] Il figlio con pid %d è terminato con codice %d \n", getpid(), pid, WEXITSTATUS(status));
   }
  exit(EXIT_SUCCESS); 
}

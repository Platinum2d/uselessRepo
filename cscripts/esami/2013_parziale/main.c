#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/wait.h>

/* struttura di dati richiesta */
typedef struct mstruct{
   char v1;
   long int v2;
}mstruct;

/* tipo di dato apposito per semplificare l'utilizzo delle pipe multiple */
typedef int pipe_t[2];

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

void bsort(mstruct *array, int len)
{
   int i, ordered = 0;
   mstruct tmp;
   
   while (len > 1 && !ordered)
   {
      ordered = 1;
      for (i = 0; i < len - 1; i++)
      {
         if (array[i].v2 > array[i + 1].v2)
	 {
	    tmp = array[i];
	    array[i] = array[i + 1];
	    array[i + 1] = tmp;
	    ordered = 0;
	 }
      }
      len--;
   }
}

void printstruct(mstruct s)
{
   zprintf(1, " v1 = %c, v2 = %ld", s.v1, s.v2);
}

/* init: funzione di inizializzazione di un array di char con le lettere dell'alfabeto inglese */
void init(char *d)
{
   int i;

   for (i = 0; i < 26; i++)
      d[i] = 'a' + i;
}

/* main : funzione contenente il codice del processo padre */
int main(int argc, char **argv) 
{
   /* variabili di iterazione dei cicli */
   int i, j;

   /* buffer di lettura e scrittura */
   mstruct bufferchild, bufferfather;

   int status; /* gestione del valore di uscita dei processi figli*/
   pid_t pid; /* variabile/array per gestire il/i pid creato/i tramite fork() */
   int child_n; /* variabili atte a contenere il numero di processi figli e nipoti da generare */
   pipe_t *p; /* variabile puntatore alla zona di memoria contenente tutte le pipe in utilizzo */
   mstruct array[26]; /* array scambiato tra le strutture dati */
   char letters[26]; /* array dedito a contenere le lettere dell'alfabeto */
   int fd; /* file descriptor */
   char cbuff; /* buffer di lettura di un singolo carattere */
   int count; /* contatore di caratteri idonei per ogni processo */

   /* inizializzo l'array delle lettere */
   init(letters);

   /* controllo che il numero dei parametri passati allo script sia appropriato */
   if (argc != 2) 
   {
      /* stampo tutti gli errori sul file avente file descriptor 2, ovvero su stderror */
      zprintf(2, "\t[%d, padre] Utilizzo : %s file\n\n", getpid(), argv[0]);
      exit(1);
   }
   
   /* stabilisco il numero di processi da creare (tanti quante le lettere dell'alfabeto inglese) */
   child_n = 26;

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
               if (j != i) close(p[j][1]);
               if (j != i - 1) close(p[j][0]);
	    }

            /* azioni del figlio */
            if (i == 0)
	    {
	       /* se mi trovo nel primo processo, imposto la struttura con valori locali, senza leggere la struttura (dovrebbe provenire dal processo P-1, che non esiste) */
	       for (j = 0; j < child_n; j++)
	       {
	          array[j].v1 = letters[j];
		  array[j].v2 = 0;
	       }
	    }
	    else
	    {
	       /* mi trovo in un processo intermedio: leggo la struttura in arrivo */
	       if (read(p[i - 1][0], array, sizeof(array)) != sizeof(array))
	       {
	          zprintf(2, "\t[%d, figlio] Errore : read() fallita\n\n", getpid());
                  exit(5);
	       }
	    }

	    /* operazioni sulla struttura dati */
	    /* apro il file passato come parametro allo script, con controllo dell'errore */
            if ((fd = open(argv[1], O_RDONLY)) < 0)
            {
               zprintf(2, "\t[%d, padre] Errore : open() fallita sul file %s\n\n", getpid(), argv[1]);
               exit(5);
            }
            
            /* leggo ogni singolo carattere del file, contando le occorrenze della lettera correntemente presa in considerazione dal processo */
	    count = 0;
	    while (read(fd, &cbuff, sizeof(char)) == sizeof(char))
	       if (cbuff == letters[i] || cbuff == letters[i] - 3) count++;

	    array[i].v2 = count;

	    /* spedisco la struttura dati al processo successivo */
	    if (write(p[i][1], array, sizeof(array)) != sizeof(array))
	    {
	       zprintf(2, "\t[%d, figlio] Errore : write() fallita\n\n", getpid());
	       exit(6);
	    }

            /* termino, chiudendo il file appena aperto */
	    close(fd);
	    exit(cbuff);
	    break;

	 /* caso di fork() fallita */
	 case -1:
	    zprintf(2, "\t[%d, padre] Errore : fork() fallita\n\n", getpid());
	    exit(7);
	    break;
      }
   }

   /* codice del padre */
   /* chiudo i lati della pipe inutilizzati dal padre */
   close(p[child_n - 1][1]);
   for (i = 0; i < child_n - 1; i++)
   {
      close(p[i][0]);
      close(p[i][1]);
   }

   /* azioni del padre */
   if (read(p[child_n - 1][0], array, sizeof(array)) != sizeof(array))
   {
      zprintf(2, "\t[%d, padre] Errore : read() fallita\n\n", getpid());
      exit(8);
   }

   /* ordino la struttura dati arrivata dall'ultimo figlio */
   bsort(array, 26);

   zprintf(1, "\n\n[%d, padre] Elaborazione terminata\n[%d, padre] Strutture ricevute:\n\n", getpid(), getpid());
   /* attendo la terminazione dei processi figli */
   for (i = 0; i < child_n; i++)
   {
      if ((pid = wait(&status)) < 0)
      {
         zprintf(2, "\t[%d, padre] Errore : wait() fallita\n\n");
	 exit(9);
      }      
      zprintf(1, "pid : %d, carattere : %c con codice ASCII %d, struttura : ", pid, WEXITSTATUS(status), WEXITSTATUS(status));
      printstruct(array[i]);
      zprintf(1, "\n");
   }

   exit(EXIT_SUCCESS); 
}

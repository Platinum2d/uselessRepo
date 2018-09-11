
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/wait.h>


/* topologia di comunicazione tra i processi tramite pipe */
/*
 		F <---------------|Pn
		|P0		  |
		V		  |
		C0 --> C1 ... --> Cn
		   P1         Pn-1
  */

/* tipo di dato apposito per semplificare l'utilizzo delle pipe multiple */
typedef int pipe_t[2];

/* struttura dati apposita */
typedef struct mstruct
{
	long int c1;
	int c2;
	long int c3;
}mstruct;

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

/* funzione apposita per stampare una struttura dati su standard output */
void printstruct(mstruct s)
{
	zprintf(1, "c1 : %ld, c2 : %d, c3 : %ld", s.c1, s.c2, s.c3);
}

int child(pipe_t *p, int child_n, int child_id, char *to_analyze, char Cx) 
{
	/* variabile di indicizzazione del ciclo for */
	int i;

	/* buffer per leggere dal figlio precedente */
	mstruct buffer;

	/* variabile per gestire il numero di byte scritti e letti dalle primitive read() e write() */
	int bw, br;

	/* variabile per contenere il file descriptor del file da analizzare */
	int fd;

	/* variabile per il conteggio delle occorrenze del carattere nel file */
	unsigned int count = 0;

	/* buffer di lettura */
	char readbuffer;
	
	/* chiusura dei lati della pipe non utilizzati dal figlio per realizzare la topologia di cui sopra */
	close(p[child_id][1]);
	close(p[child_id + 1][0]);
	for (i = 0; i < child_n; i++)
	{
		if (i != child_id && i != child_id + 1)
		{
			close(p[i][0]);
			close(p[i][1]);
		}
	}
	
	/* codice del figlio */
	/* leggo il carattere in arrivo e lo stampo */
	br = read(p[child_id][0], &buffer, sizeof(mstruct));
	if (br != sizeof(mstruct))
	{
		zprintf(2, "\t[%d] Errore : read() fallita\n", getpid());
		exit(EXIT_FAILURE);
	}

	/* stampo la struttura ricevuta */

	/* apro in sola lettura il file da analizzare */
	if ((fd = open(to_analyze, O_RDONLY)) < 0)
	{
		zprintf(2, "\t[%d] Errore : open() fallita\n", getpid());
		exit(EXIT_FAILURE);
	}

	/* leggo l'intero contenuto del file e conto le occorrenze del carattere */
	while(read(fd, &readbuffer, sizeof(char)) > 0)
		if (readbuffer == Cx) count++;
	
	/* chiudo il file */
	close(fd);

	/* invio il carattere al prossimo processo, ma solo se non sono all'interno dell'ultimo */
	if (child_id < child_n)
	{
		/* assegno a c1 il valore massimo tra quello attuale e il count appena calcolato, e a c2 il progressivo di quest'ultimo */

		if (count > buffer.c1) 
		{
			buffer.c1 = count;
			buffer.c2 = child_id;
		}

		buffer.c3 += count;

		/* scrivo al processo successivo il carattere e controllo l'esito dell'invio */	
		bw = write(p[child_id + 1][1], &buffer, sizeof(mstruct));
		if (bw != sizeof(mstruct))
		{	
			zprintf(2, "\t[%d] Errore : write() fallita\n", getpid());
			exit(EXIT_FAILURE);
		}
	}
	exit(child_id);
}


/* father : funzione contenente il codice del processo padre, lanciato dal main */
int father(pipe_t *p, int child_n, pid_t *pids) 
{
	/* variabile di indicizzazione del ciclo for */
	int i;
	
	/* variabile per gestire il numero di byte scritti e letti dalle primitive read() e write() */
	int bw, br;

	/* struttura dati S0 da inviare al primo figlio */
	mstruct s;

	/* struttura di dati che funge da buffer per leggere dal figlio precedente (o dal padre)*/
	mstruct buffer;

	int status;
	
	/* chiusura dei lati della pipe non utilizzati dal padre per realizzare la topologia di cui sopra */
	close(p[0][0]);
	close(p[child_n][1]);
	for (i = 1; i < child_n; i++) 
	{
		close(p[i][0]);
		close(p[i][1]);
	}

	/* codice del padre */
	/* inizializzo la struct da inviare */
	s.c1 = s.c2 = s.c3 = 0;

	zprintf(1, "\n[%d] inizializzo la struttura dati e la invio...\n", getpid());
	bw = write(p[0][1], &s, sizeof(mstruct));
	if (bw != sizeof(mstruct))
	{
		zprintf(2, "\t[%d] Errore : write() fallita\n\n", getpid());
		exit(7);
	}

	/* attesa dei processi figli */
	for (i = 0; i < child_n; i++) 
	{
		wait(&status);
		if (WEXITSTATUS(status) != i) exit(8);
	}

	/* leggo la struttura in arrivo dal precedente processo */
	br = read(p[child_n][0], &buffer, sizeof(mstruct));
	if (br != sizeof(mstruct))
	{
		zprintf(2, "\t[%d] Errore : read() fallita\n", getpid());
		exit(9);
	}

	zprintf(1, "[%d] struttura dati ricevuta : ", getpid());
	printstruct(buffer);
	zprintf(1, "\n\n\n[%d] il processo che ha trovato piu' spesso il carattere ha indice %d, pid %d con %d occorrenze\n\n", getpid(), buffer.c2, pids[buffer.c2], buffer.c1);

	
	exit(EXIT_SUCCESS);
}


/* main : funzione contenente il codice del processo padre */
int main(int argc, char **argv) 
{
	/* variabile di indicizzazione del ciclo for */
	int i;

	/* variabile atta a contenere il carattere passato */
	char Cx;

	/* variabile/array per gestire il/i pid creato/i tramite fork() */
	pid_t pid;

	/* varibaile atta a contenere il numero di processi figli da generare */
	int child_n;

	/* variabile puntatore alla zona di memoria contenente tutte le pipe in utilizzo */
	pipe_t *p;

	/* pid dei figli */
	pid_t *pids;
	
	/* controllo del numero di parametri passati allo script */
	if (argc < 4) 
	{
		/* tutti gli errori vengono stampati sul file avente file descriptor 2, ovvero stderror */
		zprintf(2, " Utilizzo : %s file_1 file_2 [file_3] ... [file_n] carattere\n\n", argv[0]);
		exit(1);
	}

	
	/* prelevazione e controllo sui parametri passati */
	/* il numero di processi da creare è pari al numero di parametri passati, escludendo il file stesso e il carattere finale */
	child_n = argc - 2;
	
	/* allocazione dello spazio di memoria utile a contenere i pid dei processi generati */
	pids = (pid_t *) malloc(child_n * sizeof(pid_t));

	/* controllo che l'ultimo parametro sia un singolo carattere */
	if (argv[argc - 1][1] != 0)
	{
		zprintf(2, "\t[%d] Errore : l'ultimo parametro deve essere un carattere\n", getpid());
		exit(3);
	}

	Cx = argv[argc - 1][0];

	/* creazione della zona di memoria dedicata alle pipe con controllo dell'errore */
	p = (pipe_t *) malloc(sizeof(pipe_t) * (child_n + 1));
	if (p == NULL) 
	{
		zprintf(2, " Errore : malloc() fallita \n\n");
		exit(4);
	}
	
	/* inizializzazione delle pipeline allocate attraverso la primitiva pipe() */
	for (i = 0; i < child_n + 1; i++) 
	{ 
		if (pipe(p[i]) < 0) 
		{ 
			zprintf(2, " Errore : pipe() fallita \n\n");
			exit(5);
		}
	}
	
	/* creazione dei processi figli tramite la primitiva fork() */
	for (i = 0; i < child_n; i++) 
	{ 
		pids[i]=fork(); 
		/* controllo sull'esito della fork */
		switch(pids[i]) 
		{
			case 0: 
				child(p, child_n, i, argv[i + 1], Cx);
			case -1:
				zprintf(2, " Errore : fork() fallita\n\n");
				exit(6);
		}
	}
	
	father(p, child_n, pids);
}

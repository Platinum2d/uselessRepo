#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
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


/* child : funzione contenente il codice dei processi figli del principale */
int child(pipe_t *p, int child_n, int child_id) 
{
	/* variabile di indicizzazione del ciclo for */
	int i;
	char buffer;
	int br;
	
	/* chiusura dei lati della pipe non utilizzati dal figlio per realizzare la topologia di cui sopra */
	for (i = 0; i < child_n; i++) 
	{
		close(p[i][1]);
		if (i != child_id)
			close(p[i][0]);
	}
	
	/* codice del figlio */
	br = read(p[child_id][0], &buffer, sizeof(char));
	if (br != sizeof(char))
	{
		zprintf(2, " Errore : read() fallita\n\n");
		exit(EXIT_FAILURE);
	}

	zprintf(1, "[%d] ricevuto il comando %c\n", getpid(), buffer);
	
	exit(EXIT_SUCCESS);
}


/* father : funzione contenente il codice del processo padre, lanciato dal main */
int father(pipe_t *p, int child_n) 
{
	/* variabile di indicizzazione del ciclo for */
	int i;
	int bw;
	char to_send = 's';
	int status;
	
	/* chiusura dei lati della pipe non utilizzati dal padre per realizzare la topologia di cui sopra */
	for (i = 0; i < child_n; i++) 
	{
		close(p[i][0]);
	}

	/* codice del padre */
	for (i = child_n - 1; i >= 0; i--)
	{
		zprintf(1, "[%d] invio carattere al figlio n. %d\n", getpid(), i);
		bw = write(p[i][1], &to_send, sizeof(char));
		if (bw != sizeof(char))
		{
			zprintf(2, " Errore : write() fallita\n\n");
			exit(6);
		}
	}

	/* attesa dei processi figli */
	for (i = 0; i < child_n; i++) 
	{
		wait(&status);
		if (WEXITSTATUS(status) != EXIT_SUCCESS)
		{
			exit(7);
		}
	}
	
	exit(EXIT_SUCCESS);
}


/* main : funzione contenente il codice del processo padre */
int main(int argc, char **argv) 
{
	/* variabile di indicizzazione del ciclo for */
	int i;

	/* variabile/array per gestire il/i pid creato/i tramite fork() */
	pid_t pid;

	/* varibaile atta a contenere il numero di processi figli da generare */
	int child_n;

	/* variabile puntatore alla zona di memoria contenente tutte le pipe in utilizzo */
	pipe_t *p;
	
	/* controllo dei parametri passati allo script */
	if (argc != 2) 
	{
		/* tutti gli errori vengono stampati sul file avente file descriptor 2, ovvero stderror */
		zprintf(2, " Utilizzo : %s numero_processi\n\n", argv[0]);
		exit(1);
	}
	
	/* prelevazione e controllo dei parametri passati a linea di comandi */
	child_n = atoi(argv[1]);
	if (child_n <= 0) 
	{ 
		zprintf(2, " Errore : %d deve essere un intero positivo\n\n", child_n);
		exit(2);
	}
	
	/* creazione della zona di memoria dedicata alle pipe con controllo dell'errore */
	p = (pipe_t *) malloc(sizeof(pipe_t) * child_n);
	if (p == NULL) 
	{
		zprintf(2, " Errore : malloc() fallita \n\n");
		exit(3);
	}
	
	/* inizializzazione delle pipeline allocate attraverso la primitiva pipe() */
	for (i = 0; i < child_n; i++) 
	{ 
		if (pipe(p[i]) < 0) 
		{ 
			zprintf(2, " Errore : pipe() fallita \n\n");
			exit(4);
		}
	}
	
	/* creazione dei processi figli tramite la primitiva fork() */
	for (i = 0; i < child_n; i++) 
	{ 
		pid=fork(); 
		/* controllo sull'esito della fork */
		switch(pid) 
		{
			case 0: 
				child(p, child_n, i);
			case -1:
				zprintf(2, " Errore : fork() fallita\n\n");
				exit(5);
		}
	}
	
	father(p, child_n);
}

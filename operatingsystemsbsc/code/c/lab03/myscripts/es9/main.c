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

int grandson(pipe_t *gp, int grandson_n, int grandson_id)
{
	/* variabile di indicizzazione del ciclo for */
	int i;

	/* variabili atte a contenere il numero di byte letti/scritti durante l'esecuzione dalla pipe */
	int br;
	char buffer;

	/* chiusura dei lati della pipe non utilizzati dal nipote per realizzare la topologia di cui sopra */
	for (i = 0; i < grandson_n; i++) 
	{
		close(gp[i][1]);
		if (i != grandson_id) close(gp[i][0]);
	}
	
	/* codice del nipote */
	br = read(gp[grandson_id][0], &buffer, sizeof(char));
	if (br != sizeof(char))
	{
		zprintf(2, "\t[%d, nipote] Errore : read() fallita\n\n", getpid());
		exit(13);
	}

	zprintf(1, "[%d, nipote] ricevuto il carattere %c \n", getpid(), buffer);

	exit(EXIT_SUCCESS);

}

/* child : funzione contenente il codice dei processi figli del principale */
int child(pipe_t *p, int child_n, int grandson_n, int child_id) 
{
	/* variabile di indicizzazione del ciclo for */
	int i;
	char buffer;
	pid_t pid;
	int br, bw;

	/* chiusura dei lati della pipe non utilizzati dal figlio per realizzare la topologia di cui sopra */
	for (i = 0; i < child_n; i++) 
	{
		close(p[i][1]);
		if (i != child_id) close(p[i][0]);
	}

	/* pipe del figlio per comunicare con i nipoti */
	pipe_t *gp = (pipe_t *) malloc(grandson_n * sizeof(pipe_t));
	if (gp == NULL)
	{
		zprintf(2, "\t[%d, figlio] Errore : malloc() fallita");
		exit(8);
	}
	
	/* inizializzazione delle pipe dei nipoti */
	for (i = 0; i < grandson_n; i++)
	{
		if (pipe(gp[i]) < 0)
		{
			zprintf(2, "\t[%d, figlio] Errore : pipe() fallita\n\n", getpid());
			exit(9);
		}
	}
	
	/* codice del figlio */

	/* generazione dei nipoti */
	for (i = 0; i < grandson_n; i++)
	{
		pid = fork();
		switch(pid)
		{
			case 0:
				grandson(gp, grandson_n, i);
				break;

			case -1:
				zprintf(2, "\t[%d, figlio] Errore : fork() fallita\n\n");
				exit(10);
				break;
		}
	}

	/* chiusura dei lati delle pipe dei nipoti */
	for (i = 0; i < grandson_n; i++)
	{
		close(gp[i][0]);
	}

	/* leggo il carattere in entrata e ne verifico lettura */
	br = read(p[child_id][0], &buffer, sizeof(char));
	if (br != sizeof(char))
	{
		zprintf(2, "\t[%d, figlio] Errore : read() fallita\n\n", getpid());
		exit(11);
	}
	zprintf(1, "[%d, figlio] ricevuto %c\n", getpid(), buffer);


	/* invio il carattere ai nipoti */
	for (i = 0; i < grandson_n; i++)
	{
		zprintf(1, "[%d, figlio] invio il carattere %c...\n", getpid(), buffer);
		bw = write(gp[i][1], &buffer, sizeof(char));
		if (bw != sizeof(char))
		{
			zprintf(2, "\t[%d, figlio] Errore : write() fallita\n\n", getpid());
			exit(12);
		}
	}

	for (i = 0; i < grandson_n; i++) 
		wait(NULL);

	exit(EXIT_SUCCESS);
}


/* father : funzione contenente il codice del processo padre, lanciato dal main */
int father(pipe_t *p, int child_n) 
{
	/* variabile di indicizzazione del ciclo for */
	int i;

	/* variabile atta a contenere il numero di byte scritti ai figli */
	int bw;

	char to_send = 'a';

	/* chiusura dei lati della pipe non utilizzati dal padre per realizzare la topologia di cui sopra */
	for (i = 0; i < child_n; i++) 
	{
		close(p[i][0]);
	}

	/* codice del padre */
	for (i = 0; i < child_n; i++)
	{
		zprintf(1, "[%d] Invio il carattere %c...\n", getpid(), to_send);
		bw = write(p[i][1], &to_send, sizeof(char));
		if (bw != sizeof(char))
		{
			zprintf(2, "\t[%d] Errore : write() fallita\n\n", getpid());
			exit(7);
		}
	}

	/* attesa dei processi figli */
	for (i = 0; i < child_n; i++) 
	{
		wait(NULL);
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

	/* varibaili atte a contenere il numero di processi figli e nipoti da generare */
	int child_n;
	int child_m;

	/* variabile puntatore alla zona di memoria contenente tutte le pipe in utilizzo */
	pipe_t *p;
	
	/* controllo dei parametri passati allo script */
	if (argc != 3) 
	{
		/* tutti gli errori vengono stampati sul file avente file descriptor 2, ovvero stderror */
		zprintf(2, " Utilizzo : %s numero_figli numero_nipoti\n\n", argv[0]);
		exit(1);
	}
	
	/* prelevazione e controllo dei parametri passati a linea di comando */
	child_n = atoi(argv[1]);
	child_m = atoi(argv[2]);
	if (child_n <= 0) 
	{ 
		zprintf(2, " Errore : %d deve essere un intero positivo\n\n", child_n);
		exit(2);
	}
	
	if (child_m <= 0)
	{
		zprintf(2, " Errore : %d deve essere un intero positivo\n\n", child_m);
		exit(3);
	}	
	
	/* creazione della zona di memoria dedicata alle pipe con controllo dell'errore */
	p = (pipe_t *) malloc(sizeof(pipe_t) * child_n);
	if (p == NULL) 
	{
		zprintf(2, " Errore : malloc() fallita \n\n");
		exit(4);
	}
	
	/* inizializzazione delle pipeline allocate attraverso la primitiva pipe() */
	for (i = 0; i < child_n; i++) 
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
		pid=fork(); 
		/* controllo sull'esito della fork */
		switch(pid) 
		{
			case 0: 
				child(p, child_n, child_m, i);
			case -1:
				zprintf(2, " Errore : fork() fallita\n\n");
				exit(6);
		}
	}
	
	father(p, child_n);
}

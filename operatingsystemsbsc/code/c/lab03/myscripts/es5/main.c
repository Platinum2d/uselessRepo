
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/wait.h>

/* 
	topologia di comunicazione dei processi	
			F<-----------
			^   |       |
			|   |       |
			|   |       |
			C1  C2      Cn
*/

/* zprintf: funzione che utilizza le primite di Unix per replicare la funzione fprintf() */
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

/* tipo di dato personalizzato */
struct foo
{
   int n;
};

/* tipo di dato per gestire più facilmente i file descriptor creati dalle pipe */
typedef int pipe_t[2];

/* funzione del proceso figlio */
int child(pipe_t p, int child_n, int child_id) 
{
   struct foo f;
   f.n = child_id;

   close(p[0]);
   write(p[1], &f, sizeof(struct foo));

   exit(EXIT_SUCCESS);
}

/* funzione del processo chiamante */
int father(pipe_t p, int child_n) 
{
   unsigned int i;
   struct foo buffer;

   close(p[1]);
   for (i = 0; i < child_n; i++)
   {
      wait(NULL);
      read(p[0], &buffer, sizeof(struct foo));
      zprintf(1, "%d\n", buffer.n);
   }


   exit(EXIT_SUCCESS);
}


/* funzione principale */
int main(int argc, char **argv) 
{
	/* variabile di iterazione dei cicli for */
	int i;

	/* variabile per gestire i pid delle fork */
	pid_t pid; 

	/* variabile per gestire il numero di processi figli da creare */
	int child_n;

	/* variabile per gestire i file descriptor generati dalle pipe */
	pipe_t p;

	/* controllo che il numero dei parametri sia conforme a quanto assegnato */
	if (argc != 2) {
		zprintf(1, " Utilizzo : %s numero_processi\n\n", argv[0]);
		exit(1);
	}

	child_n = atoi(argv[1]);
	
        if(pipe(p) < 0)
	{
           zprintf(2, " Errore : pipe fallita");
	   exit(2);
	}

	for(i = 0; i < child_n; i++)
	{
           pid = fork();
	   switch(pid)
	   {
	      case 0:
		      child(p, child_n, i);
		      break;

	      case -1:
		      zprintf(2, " Errore : fork fallita\n\n");
		      exit(3);
		      break;
	   }
	}
  
	father(p, child_n);
}


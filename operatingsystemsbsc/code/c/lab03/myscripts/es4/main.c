#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/wait.h>

/* 
	topologia di comunicazione dei processi	
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
struct person
{
   char nome[20];
   char cognome[20];
   unsigned short eta;
};

/* tipo di dato per gestire più facilmente i file descriptor creati dalle pipe */
typedef int pipe_t[2];

/* funzione del proceso figlio */
int child(pipe_t p) 
{
   struct person buffer;
   close(p[1]);
   read(p[0], &buffer, sizeof(struct person));
   zprintf(1, "[%d] nome: %s\ncognome: %s\neta:%hu\n\n", getpid(), buffer.nome, buffer.cognome, buffer.eta);
   exit(EXIT_SUCCESS);
}


/* funzione del processo chiamante */
int father(pipe_t p, struct person to_write) 
{
   close(p[0]);
   write(p[1], &to_write, sizeof(struct person));
   wait(NULL);
   exit(EXIT_SUCCESS);
}


/* funzione principale */
int main(int argc, char **argv) {

   /* variabile per gestire i pid delle fork */
   pid_t pid; 

   pipe_t p;
	
   struct person ps;
   strcpy(ps.nome, "Marco");
   strcpy(ps.cognome, "Rossi");
   ps.eta = 21;
   
   if (pipe(p) < 0)
   {
      zprintf(2, " Errore : pipe fallita\n\n");
      exit(1);
   }

   pid = fork();
   switch(pid)
   {
      case 0:
	      child(p);
	      break;

      case -1:
	      zprintf(2, " Errore : fork fallita\n\n");
	      exit(2);
	      break;
   }
   
   father(p, ps);
}

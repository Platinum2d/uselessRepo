
/* Includo le librerie */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "utils.h"

/* Funzione che stabilisce la dimensione del file scorrendolo carattere per carattere */
unsigned int getFileSize(char *filepath)
{
   char buffer;
   unsigned int count = 0;
   int fd = open(filepath, O_RDONLY);

   while (read(fd, &buffer, sizeof(char)) > 0) count++;

   close(fd);
   return count;
}

/* Codice del figlio */
void child(char *filepath)
{
   /* Nel figlio, ricavo la dimensione e la stampo a video. */
   int countedByChild = getFileSize(filepath);
   zprintf(1, "[%d] Byte letti dal figlio : %u\n", getpid(), countedByChild);
   exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
   /* Variabile per gestire i pid ritornati dalle varie funzioni */
   pid_t pid;
   
   /* Effettuo il controllo sul numero dei parametri: devono essere almeno n (incluso l'elemento di posizione 0) */
   if (argc < 2)
   {
      zprintf(2, " Utilizzo : %s percorso_file \n\n", argv[0]);
      exit(1);
   }

   /* Controllo che il file specificato sia leggibile (viene fatto prima dell'esecuzione parallela per evitare output indesiderato)*/
   int fd = open(argv[1], O_RDONLY);
   if (fd < 0)
   {
      zprintf(2, " Errore : impossibile aprire il file %s\n", argv[1]);
      exit(2);
   }
   close(fd);

   /* Effettuo la fork e controllo che sia andata a buon fine */
   pid = fork();
   switch(pid)
   {
      case 0:
	      child(argv[1]);
	      break;

      case -1:
	      zprintf(2, " Errore : fork fallita\n\n");
	      exit(3);
	      break;
   }
   
   /* Ricavo la dimensione in byte del file all'interno del processo padre */
   unsigned int countedByFather = getFileSize(argv[1]);
   zprintf(1, "[%d] Byte letti dal padre : %u\n", getpid(), countedByFather);

   exit(EXIT_SUCCESS);
}

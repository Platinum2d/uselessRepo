#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "utils.h"

/* struttura : N numero caratteri, F file, K posizione */
int main(int argc, char **argv)
{
   /* utilizzo una apposita variabile variabile intera per il file descriptor */
   int fd;

   /* controllo il numero di parametri */
   if (argc < 4)
   {
      zprintf(1, " Utilizzo: %s numero_caratteri percorso_file posizione_partenza\n\n", argv[0]);
      exit(1);
   }

   int N=atoi(argv[1]), K=atoi(argv[3]);

   /* controllo che N sia minore di 100 e positivo */
   if (N >= 100)
   {
      zprintf(1, " Errore : %d non è minore di 100\n\n", argv[1]);
      exit(2);
   }
   else
   {
      if (N < 0)
      {
         zprintf(1, " Errore : %d è negativo\n\n", argv[1]);
         exit(3);
      }
   }

   /* provo ad aprire in lettura in lettura e scrittura il file */
   if ( (fd = open(argv[2], O_RDWR)) < 0)
   {
      zprintf(1, " Errore : impossibile aprire %s\n\n", argv[2]);
      exit(4);
   }

   /* controllo che K sia positivo e sia un numero */
   int i;
   for (i = 0; i < strlen(argv[3]) - 1; i++)
   {
      if (argv[3][i] < '0' || argv[3][i] > '9')
      {
	 zprintf(1, " Errore : %s non è un numero intero positivo\n\n", argv[3]);
         exit(5);
      }
   }

   if (K < 0)
   {
      zprintf(1, " Errore : %s non è un numero intero positivo\n\n", argv[3]);
      exit(6);
   }

   /* leggo i primi N caratteri dal file attraverso un apposito buffer*/
   char *buffer = malloc(N * sizeof(char));
   read(fd, buffer, N * sizeof(char));;

   /* controllo che la posizione K non sfori la dimensione del file */
   if ( lseek(fd, K, SEEK_SET) < 0 )
   {
      zprintf(1, " Errore : impossibile spostare il file puntare al byte %d", K);
      exit(7);
   }

   /* dopo aver spostato il file pointer, scrivo gli N byte letti */
   write(fd, buffer, N * sizeof(char));

   /* dealloco il buffer e chiudo il file */
   free(buffer);
   close(fd);

   exit(EXIT_SUCCESS);
}

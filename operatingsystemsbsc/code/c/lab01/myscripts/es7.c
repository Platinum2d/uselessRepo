#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "utils.h"

/* N F*/

int main(int argc, char **argv)
{
   /* variabile per contenere il numero di linee, di default a 10 */
   int lines = 10;

   /* variabile per gestire il file descriptor */
   int fd;

   /* controllo che il numero di parametri sia almeno 1*/
   if (argc < 2 || argc > 3)
   {
      zprintf(1, " Utilizzo : %s [numero_linee] file\n\n", argv[0]);
      exit(1);
   }

   /* controllo, se specificato, che il numero di linee sia positivo */
   if (argc == 3)
   {
      lines = atoi(argv[1]);
      if (lines < 0)
      {
         zprintf(1, " Errore : %d non è positivo\n\n", lines);
	 exit(2);
      }
      argv[1] = argv[2];
   }

   /* controllo che file sia apribile e, nel caso, lo apro*/
   if ( (fd = open(argv[1], O_RDONLY)) < 0)
   {
      zprintf(1, " Errore : impossibile aprire %s\n\n", argv[1]);
      exit(3);
   }
   
   /* utilizzo una variabile per contare le linee correntemente stampate */
   int printedlines = 0;

   /* comincio a scorrere il file byte per byte */
   while(1)
   {
      /* carattere-buffer per la lettura */
      char c;

      int nread = read(fd, &c, 1);
      /* se il carattere corrente è un a capo, significa che sono arrivato a fine riga */
      
      if (nread == 0) break;
      
      write(1, &c, 1);

      if (c == '\n')
      {
	 /* incremento il contatore delle linee stampate: se è pari o superiori a quelle specificate, finisco di stampare*/
         printedlines++;
	 if (printedlines >= lines)
            break;
      }
   }

   exit(EXIT_SUCCESS);
}


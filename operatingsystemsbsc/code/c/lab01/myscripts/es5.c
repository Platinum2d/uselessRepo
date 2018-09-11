#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "utils.h"
int main(int argc, char **argv)
{
   /* variabile utilizzata per il file descriptor */
   int fd;

   /* controllo se il numero di parametri è appropriato */
   if (argc < 2)
   {
      zprintf(1, " Utilizzo : %s percorso_file\n\n", argv[0]);
      exit(1);
   }

   /* apro il file e ne gestisco l'errore */
   if ( (fd = open(argv[1], O_RDWR)) < 0 )
   {
      zprintf(1, " Errore : impossibile aprire %s\n\n", argv[1]);
      exit(2);
   }
   
   /* sovrasrivo i primi 5 caratteri con la stringa Hello */
   write(fd, "Hello", 5 * sizeof(char));

   /* tronco il file ai primi 16 caratteri */ 
   truncate(argv[1], 16 * sizeof(char));

   /* chiudo il file ed esco */
   close(fd);

   exit(EXIT_SUCCESS);
}

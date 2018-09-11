#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "utils.h"

int main(int argc, char **argv)
{
   /* variabile intera per contenere il file descriptor */
   int fd;

   /* controllo sul numero di parametri */
   if (argc < 2)
      fd = 1;
   else 
   {
      /* controllo sulla validità del parametro */
      if (argv[1][1] != 0 || argv[1][0] > '2' || argv[1][0] < '1')
         fd = 1;
      else
      {
         /* scelgo quale file descriptor usare */
         fd = argv[1][0] - '0';
      }
   }
   
   zprintf(fd, "Hello World! [%d]\n", fd);

   exit(EXIT_SUCCESS);
}

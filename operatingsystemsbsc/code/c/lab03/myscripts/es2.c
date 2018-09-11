

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

int main(int argc, char **argv)
{
   /* Array di 2 interi apposito per contenere i file descriptors generati dalla pipe */
   int piped[2];

   pipe(piped);
   char buffer = 'a';
   unsigned int written = 0;
   close(piped[1]);
   while(write(piped[1], &buffer, sizeof(char)) > 0) written++;

   zprintf(1," [%d] Scritti %u bytes\n\n", getpid(), written);

   exit(EXIT_SUCCESS);
}

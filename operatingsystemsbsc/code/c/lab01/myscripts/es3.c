#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "utils.h"

int main(int argc, char **argv)
{
   unsigned int i;

   for (i = 1; i < argc; i++)
   {
      fprintf(stdout, "%s\n", argv[i]);
   }

   exit(EXIT_SUCCESS);
}

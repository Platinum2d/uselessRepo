#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"

int main(int argc, char **argv)
{
   int i = 1;
   char c = 'a';
   char array[1024] = "provaaa";
   char *pointerarray = malloc(1024 * sizeof(char));
   strcpy(pointerarray, "ciaoborel");
   
   fprintf(stdout, "intero: %d\ncarattere: %c\narray: %s\n", i, c, array);

   exit(EXIT_SUCCESS);
}


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "utils.h"

int main(int argc, char **argv)
{
   char buf[] = "Hello World!\n";

   printf("%d", argc);

   fprintf(stdout, "%s", buf);
   write(1, buf, sizeof(buf));
   zprintf(1, buf);
   return EXIT_SUCCESS;
}

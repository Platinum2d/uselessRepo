#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main(void)
{
   int fd = open("prova.txt", O_RDONLY);
   char buffer;

   lseek(fd, 0, SEEK_CUR);
   
   read(fd, &buffer, sizeof(char));
   printf(" Ho letto %c\n", buffer);

   exit(EXIT_SUCCESS);
}

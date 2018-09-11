
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

/* Funziona apposita contenente il codice dei figli, atta a separare quest'ultimo dal codice del padre */
void child()
{
   /* Tutto è andato come previsto: esco dal processo figlio con codice zero */
   exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
   /* Array di 2 interi apposito per contenere i file descriptors generati dalla pipe */
   int piped[2];

   pipe(piped);
   zprintf(1, "[%d] File descriptors generati : %d, %d\n\n", getpid(), piped[0], piped[1]);

   exit(EXIT_SUCCESS);
}

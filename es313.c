#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

typedef int pipe_t[2];

#define NPRIMES 8
struct test_primes {
	int numbers[NPRIMES];
	char is_prime[NPRIMES];
};

int main(int argc, char **argv){

	/*----variabili locali-----*/
	int pid;
	int N; /* numero di processi figli (param.) */
	int i, j, primo;
	pipe_t *p;
	pipe_t *p2;
	int status;
	struct test_primes t;

	/*-------------------------------*/
	
	/* controllo che ci sia esattamente 1 parametro  */
	if(argc != 2){
		printf("Errore: deve esserci un parametro\n");
		exit(1);
	}

	N = atoi(argv[1]);

	if(N<=0){
		printf("Errore: il numero di figli deve essere positivo\n");
		exit(6);
	}

	p = (pipe_t *)malloc(N*sizeof(pipe_t));
	p2 = (pipe_t *)malloc(N*sizeof(pipe_t));
	/* -- creazione N pipe -- */
	for(i=0; i<N; i++){
		if(pipe(p[i])<0){
			printf("Error pipe %d\n",i);
			exit(7);
		}
		if(pipe(p2[i])<0){
			printf("Error pipe2 %d\n", i);
			exit(8);
		}
	}

	//per ogni figlio faccio la fork e apro una comunicazione pipe
	for(i=0; i<N; i++){

		/* faccio N fork */
		pid = fork();

		if(pid<0){
		       	/* -------------- error fork --------------------- */
			printf("Errore fork!\n");
			exit(3);
		}

		if(pid==0){ 

			/* -------------- codice figlio ------------------ */

			// ciascun figlio chiude gli output e fa in modo che venga aperto solo un
			// input, ovvero che ci sia solo una pipe attiva in questo momento
			//
			for(j=0; j<N; j++){
				close(p[j][1]);
				close(p2[j][0]);
				//tengo aperta la porta di input solo del figlio i-esimo corrente
				if(j!=i){
				       	close(p[j][0]);
					close(p2[j][1]);
				}
			}

						
			//lettura da pipe
			//printf("CHILD %d reading..\n",i);
			if(read(p[i][0],&t,sizeof(struct test_primes))!=sizeof(struct test_primes)){
				printf("Errore nella lettura dalla pipe2\n");
				exit(4);
			}
			
			primo=1;			
			//controllo che l'iesimo numero della struttura ricevuta sia primo o no
			if(t.numbers[i] == 1 || t.numbers[i] == 0){
				primo = 0; //no primi
			} else{
				for(j=2; j<t.numbers[i]; j++){
					if(t.numbers[i] % j == 0){
						primo = 0; //no primi
					}
				}
			}

			if(primo == 1){
				t.is_prime[i] = 1;
			}else if(primo == 0){
				t.is_prime[i] = 0;
			}
			

			//Scrittura nella pipe2 al padre
		     if (write(p2[i][1],&t,sizeof(struct test_primes))!=sizeof(struct test_primes)){
				printf("CHILD: Errore nella scrittura su pipe2\n");
				exit(24);
			}
		     printf("CHILD %d writes number %d (%c)!\n", i, t.numbers[i], t.is_prime[i]);

			exit(0);
		}
	}

	/* --------------- codice padre ---------------- */
	
	/* chiusura lati che non usa su tutti gli N pipe */
	for(i=0; i<N;i++){
		close(p[i][0]);
		close(p2[i][1]);
	}

	printf("prima:\n");
	for(i=0; i<NPRIMES; i++){
		t.numbers[i] = i;
		t.is_prime[i] = 1;
		printf("%d (%c) ", t.numbers[i], t.is_prime[i]);
	}

	printf("\n");

	

	//dall'ultimo al primo leggo le pipe in input e stampo il risultato
	for(i=0; i<N; i++){		
		printf("FATHER is writing to child %d\n", i);		
		/* -- scrittura su ciascuna pipe ---- */
		if(write(p[i][1], &t, sizeof(struct test_primes)) != sizeof(struct test_primes)){
			printf("errore nella scrittura su pipe!\n");
			exit(6);
		}	
			
	}


	for(i=0;i<N; i++){
		if(read(p2[i][0], &t, sizeof(struct test_primes)) != sizeof(struct test_primes)) {
			printf("Errore nella lettura da pipe 2 (FATHER)!\n");
			exit(11);
		}
		printf("numero %d primo=%c\n", t.numbers[i], t.is_prime[i]);
	}

	//attesa della terminazione degli N figli
	for(i=0; i<N; i++){
		pid=wait(&status);
		if(pid<0){
			printf("Errore wait\n");
			exit(9);
		}
		if((status & 0xFF) != 0){
			printf("Figlio con pid %d terminato in modo anomalo!\n", pid);
			exit(10);
		}else{
			//int ret = (int)((status >> 8)& 0xFF); //bastava WEXITSTATUS(status)
			//printf("Il figlio con pid %d ha ritornato %d\n",pid, ret);
		}
	}
	exit(0);
}

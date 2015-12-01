#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include<string.h>

char **buffer1;
char buffer2[5][11];
sem_t espacioB1;
sem_t datoB1;
sem_t espacioB2;
sem_t datoB2;

int palindromo (char *palabra) {
 	int esPalindromo = 1; 
	int i,j;

	j=strlen(palabra)-1;
	for (i=0; i<strlen(palabra)/2 && esPalindromo; i++, j--) {
		if (*(palabra+i)!=*(palabra+j)) {
			esPalindromo = 0; 
		}
	}
	return esPalindromo;
}

void *productor(void *arg){

	FILE *archivo;
	char **argumentos = (char **) arg;
	char *nombreFich = argumentos[2];
	int tam = atoi(argumentos[1]);	
	char palabra[10];
	
	
	printf("%d\n", tam);
	int contador = 0;
	archivo = fopen(nombreFich,"r");
	while(fgets(palabra, 10, archivo)!= NULL){
		printf("prod1\n"); fflush(stdout);
		sem_wait(&espacioB1);
		printf("prod2\n"); fflush(stdout);
		sprintf(buffer1[contador],"%s", palabra);
		sem_post(&datoB1);
		if(contador<tam-1){
			contador++;
		}else{
			contador=0;
		}
		
	}
	pthread_exit(0);
}

void *consumidor1 (void *arg){
	char ** argumentos = (char**) arg;
	int tam = atoi(argumentos[1]);
	int i,j=0;
	char palabra[8];
	//Coge palabra, cuantas veces?
	while(1>0){
		//falta tami
		for(i=0;i<tam;i++,j++){
			printf("antescon\n");
			sem_wait(&datoB1);
			printf("despuescon\n");
			if(palindromo(buffer1[i])){
				sprintf(palabra,"pal si");	
			}else{
				sprintf(palabra,"pal no");
			}
			sem_post(&espacioB1);
			if(j<5){
				sprintf(buffer2[j],"%s",palabra);
				
			}else{
				j=0;
				sprintf(buffer2[j],"%s",palabra);
			}				
		}

	pthread_exit(0);
	}
}

int main(int argc, char *argv[]){
	int *b1;
	int tam = (int) *argv[1];
	int i=0;
	
	sem_init(&datoB1,0,0);
	sem_init(&espacioB1,0,tam);

	if((buffer1=(char**)malloc(tam*sizeof(char*)))==NULL){
		printf("ERROR al reservar memoria\n");
		return 1;
	}
	for(i=0;i<tam;i++){
		if((buffer1[i]=(char*)malloc(11*sizeof(char)))==NULL){
			printf("ERROR al reservar memoria\n");
			return 1;
		}
	}	
	pthread_t tid[3];
	pthread_create(&tid[0], NULL, productor, (void*) argv);
	pthread_create(&tid[1], NULL, consumidor1, (void*) argv);
	pthread_join(tid[0], NULL);
	pthread_join(tid[1],NULL);
	for (i=0;i<tam;i++){
		printf("%s", buffer1[i]);
	}
	for (i=0;i<5;i++){
		printf("%s\n", buffer2[i]);
	}
	sem_destroy(&datoB1);
	sem_destroy(&espacioB1);
}

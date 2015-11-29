#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>

char buffer1[10][7];
char buffer2[5][11];
sem_t semaforo1;
sem_t semaforo2;

void *productor(void *arg){
	FILE *archivo;
	char **argumentos = (char **) arg;
	char *nombreFich = argumentos[2];
	
//	sprintf(nombreFich,"./%s",nombreFich);
	int tam = *((int *) &argumentos[1]);
	int contador=0;
	char palabra[11];
	
	archivo=fopen(nombreFich,"r");
		
	//sem_wait(&semaforo1);
	while(fgets(buffer1[contador],10,archivo)!=NULL){
		
		if(contador<tam-2){
			contador++;
		} else {
			contador =0;
		}

	}
	//sem_post(&semaforo1);
	pthread_exit(0);
}
int  main( int argc, char *argv[]){
	int tam = (int) *argv[1];
	int i;
	sem_init(&semaforo1,0,tam);
	pthread_t tid[3];
	pthread_create(&tid[0],NULL,productor,(void *) argv);
	pthread_join(tid[0],NULL);
	/*for(i=0;i<4;i++){
		printf("%s",buffer1[i]);
	}*/
}

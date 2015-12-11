#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include<string.h>

char **buffer1;
char buffer2[5][11];
int x=0;
int y=0;
int tamFich=0;
sem_t espacioB1;
sem_t datoB1;
sem_t espacioB2;
sem_t datoB2;
sem_t mutexI;
sem_t mutexJ;

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
	int tmp;
	char indice=0;
	
	int contador = 0;
	archivo = fopen(nombreFich,"r");
	do{
		
		sem_wait(&espacioB1);
		tmp = fscanf (archivo,"%s",buffer1[contador]);
		tamFich++;
		sem_post(&datoB1);
		contador=(contador+1)%tam;		

	}while (tmp!=EOF);
		
	pthread_exit(0);
}

void *consumidor1 (void *arg){
	printf("hilo nuevo");
	char ** argumentos = (char**) arg;
	int tam = atoi(argumentos[1]);
	int tmp=0;
	char palabra[20];
	//Coge palabra, cuantas veces?

	while(tamFich!=0){
		
		
		sem_wait(&datoB1);
		sem_wait(&mutexI);
		if(palindromo(buffer1[x])){
			sprintf(palabra,"%s si", buffer1[x]);	
		}else{
			sprintf(palabra,"%s no", buffer1[x]);
		}
		x=(x+1)%tam;
		tamFich--;
		printf("i: %i\n",x);fflush(stdout);
		sem_post(&mutexI);
		sem_post(&espacioB1);
		
		sem_wait(&espacioB2);
		sem_wait(&mutexJ);
		sprintf(buffer2[y],"%s",palabra);			
		y=(y+1)%5;
		printf("j: %i\n",y);fflush(stdout);
		sem_post(&mutexJ);
		sem_post(&datoB2);
			
	}
	sem_wait(&espacioB2);
	x--;
	if(x<0)
		x=tam;
	y--;
	if(y<0)
		y=5;
	sprintf(buffer2[y],"FIN");
	sem_post(&datoB2);
	printf("sale");fflush(stdout);
	pthread_exit(0);
}

void *consumidor2 (){
        int i = 0;
        char palabra[20];
	*palabra='\0';
	FILE *archSalida;
	archSalida = fopen("archivo_Salida.txt","w");
        do{
		printf("petansa");
                sem_wait(&datoB2);
                sprintf(palabra,"%s",buffer2[i]);
		sem_post(&espacioB2);
                if(strcmp(palabra,"FIN")!=0)
			fprintf(archSalida,"%s es un palindromo.\n",palabra);
                i=(i+1)%5;
        }while(strcmp(palabra,"FIN")!=0);
	fclose(archSalida);
	pthread_exit(0);
}


int main(int argc, char *argv[]){
	int *b1;
	int tam = atoi( argv[1]);
	int i=0;
	int numProd = atoi(argv[3]);

	sem_init(&datoB1,0,0);
	sem_init(&espacioB1,0,tam);
	sem_init(&datoB2,0,0);
	sem_init(&espacioB2,0,5);
	sem_init(&mutexI,0,1);
	sem_init(&mutexJ,0,1);	

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

	pthread_t tid[2+numProd];
	pthread_create(&tid[0], NULL, productor, (void*) argv);
	pthread_create(&tid[1], NULL, consumidor2, (void*) NULL);
	for(i=0;i<numProd;i++){
		pthread_create(&tid[i+2], NULL, consumidor1, (void*) argv);
	}
	for(i=0;i<numProd+2;i++)
		pthread_join(tid[i], NULL);

	
	sem_destroy(&datoB1);
	sem_destroy(&espacioB1);
	sem_destroy(&datoB2);
	sem_destroy(&espacioB2);
}

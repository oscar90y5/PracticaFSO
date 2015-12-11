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
	int tmp;
	char indice=0;
	
	printf("%d\n", tam);
	int contador = 0;
	archivo = fopen(nombreFich,"r");
	do{
		
		sem_wait(&espacioB1);
		tmp = fscanf (archivo,"%s",buffer1[contador]);
		sem_post(&datoB1);
		printf("t%s",buffer1[contador]);
		contador=(contador+1)%tam;		

	}while (tmp!=EOF);
	sem_wait(&espacioB1);
	sprintf(buffer1[contador],"FIN");	
	sem_post(&datoB1);
		
	
	
	pthread_exit(0);
}

void *consumidor1 (void *arg){
	char ** argumentos = (char**) arg;
	int tam = atoi(argumentos[1]);
	int i=0,j=0,tmp=0;
	char palabra[20];
	//Coge palabra, cuantas veces?

	while(strcmp(buffer1[i],"FIN")!=0){
		printf("3\n");
		sem_wait(&datoB1);
		if(palindromo(buffer1[i])){
			sprintf(palabra,"%s si", buffer1[i]);	
		}else{
			sprintf(palabra,"%s no", buffer1[i]);
		}
		sem_post(&espacioB1);
		
		sem_wait(&espacioB2);
		sprintf(buffer2[j],"%s",palabra);			
		sem_post(&datoB2);
		
		
		j=(j+1)%5;
		i=(i+1)%tam;
		

	
	}
	sem_wait(&espacioB2);
	j--;
	if(j<0)
		j=5;
	sprintf(buffer2[j],"FIN");
	sem_post(&datoB2);
	pthread_exit(0);
}

void *consumidor2 (){
        int i = 0;
        char palabra[20];
	*palabra='\0';
	FILE *archSalida;
	archSalida = fopen("archivo_Salida.txt","w");
        do{
                sem_wait(&datoB2);
                sprintf(palabra,"%s",buffer2[i]);
		sem_post(&espacioB2);
		printf("%s\n",palabra);
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
	
	sem_init(&datoB1,0,0);
	sem_init(&espacioB1,0,tam);
	sem_init(&datoB2,0,0);
	sem_init(&espacioB2,0,5);
	
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
	pthread_create(&tid[2], NULL, consumidor2, (void*) NULL);
	
	for(i=0;i<3;i++)
		pthread_join(tid[i], NULL);

	
	sem_destroy(&datoB1);
	sem_destroy(&espacioB1);
	sem_destroy(&datoB2);
	sem_destroy(&espacioB2);
}

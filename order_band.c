#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SIZE_B    3  /* band size*/
#define SIZE_C    10      /* column size */ 
#define SIZE_D	  10      /*dispensador size*/

bool sflag = false; 
bool pflag = false; 
bool rflag = false; 
bool mflag = false; 

int preparation_band[SIZE_B][SIZE_C]; //Matriz con las cantidad de cintas y sus respectivos ingredientes
int status_band [SIZE_B]; // 0 en reposo (sin orden), 1 en ejecución, 2 pausado

char list_ingredients [SIZE_C][20] = {"Bread", "Meat", "Tomato", "Onion", "Ham", "Egg", "Lettuce", "Mayo", "Baccon" ,"French-Fries"}; 

void message_help();
void print_preparation_bands (int preparation_band[SIZE_B][SIZE_C]);
void change_state(int pos_band, int status, int status_band [SIZE_B]);
void print_status_bands(int status_band [SIZE_B]);

int main(int argc, char  *argv[]){

	char *cvalue = NULL;
	char opt;
	int pos_band, opt_band;

	while( (opt= getopt(argc, argv, "hsp:r:m:")) != -1 ){
		switch(opt){
			case 's':
				sflag = true;
				opt_band=1;
				break;
			case 'p':
				pflag = true;
        		cvalue = optarg;
				opt_band=2;
				break;
			case 'm':
				mflag = true;
        		cvalue = optarg;
				opt_band=3;
				break;
			case 'r':
				rflag = true;
        		cvalue = optarg;
				opt_band=4;
				break;
			case 'h':
			default:
				message_help();
				return 0;
		}
	}

	if (cvalue!=NULL){
		pos_band = atoi(cvalue);
		if ( pos_band<0 || pos_band>2 ){
			printf("Ingrese un valor númerico del 0-2!\n");
			return -1;
		}
	}
	

	int sockfd;
	int len, rc ;
	struct sockaddr_in address;
	int result;

	// -- Inicio de la conexion -- 
  	//Create socket for client. 
  	//Apertura de un socket
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) { 
		perror("Socket create failed.\n") ; 
		return -1 ; 
	} 

	//Name the socket as agreed with server.
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(7734);
	len = sizeof(address);
	//Solicitar conexión con el servidor
	result = connect(sockfd, (struct sockaddr *)&address, len);

	if(result == -1){
		perror("Error has occurred");
		exit(-1);
	}

	write(sockfd, &opt_band, sizeof(opt_band)); //0. Enviamos la action opt
	read(sockfd, &preparation_band, sizeof(preparation_band)); //0.1 Leemos la preparation_band
  	read(sockfd, &status_band, sizeof(status_band)); //0.2 Leemos el status de la banda
  	//0 en reposo (sin orden), 1 en ejecución, 2 pausado
  	if(sflag){
  		print_preparation_bands(preparation_band);
  		print_status_bands(status_band);
  		
  	}else if (pflag){ //Pausar banda = 2, id=2
  		change_state(pos_band, 2, status_band );
  		write(sockfd, &status_band, sizeof(status_band)); //Enviamos el nuevo estado de la banda
  	}else if(rflag){ //Reanudar banda = 0, id=4
  		change_state(pos_band, 0, status_band );
  		write(sockfd, &status_band, sizeof(status_band)); //Enviamos el nuevo estado de la banda
  	}else if (mflag){ //Dar mantenimiento banda, id=3
  		if(status_band[pos_band]!= 2){
  			printf("Error, banda en reposo o en ejecución. Primero pause la banda #%i\n", pos_band);
  			return -1;
  		}
  		printf("\t\t---MANTENIMIENTO---\n");
  		print_preparation_bands(preparation_band);
  		printf("Bandar #%i a rellenar:\n",pos_band);
  		for (int i = 0; i < SIZE_C; i++){
  			int cantidad =0;
  			int cantidadTotal = preparation_band[pos_band][i];
  			int cantidadFaltante = SIZE_D - cantidadTotal;
  			do{
  				printf("Rellenar %s, valor actual = %i, valor faltante = %i, su valor =", list_ingredients[i], cantidadTotal,  cantidadFaltante);
				scanf("%i",&cantidad); //Leyendo el número a rellenar
				printf("\n");
				if(cantidad<0 || cantidad> SIZE_D || (cantidadTotal+cantidad)> SIZE_D)
					printf("├Ingrese valores mayores o iguales a 0 o que no sobrepase el límite de dispensadores!┤\n");

  			}while(cantidad<0 || cantidad> SIZE_D || (cantidadTotal+cantidad)> SIZE_D);

  			preparation_band[SIZE_B][SIZE_C] += cantidad;
  		}
  		write(sockfd, &preparation_band, sizeof(preparation_band)); //Enviamos la actualización de los dispensadores		
  		printf("Cambios realizados con éxito\n");

  	}

	close(result); //Cerramos la conexion con el socket
	return 0;
}


/*Cambia el estado de una banda segùn el parámetro ingresado*/
void change_state(int pos_band, int status, int status_band [SIZE_B]){
	status_band[pos_band] = status;
}

/*Imprime los valores de la matriz preparacion de banda */
void print_preparation_bands (int preparation_band[SIZE_B][SIZE_C] ){
    int i, j;
    int first;
    for (i = 0; i < SIZE_B; i++) {
      printf("Banda #%i:  ", (i));
      printf ("[");
      first = 1;
      for (j = 0; j < SIZE_C; j++) {
          if (!first)
              printf (",");
          printf ("%i", preparation_band[i][j]);
          first = 0;
      }
      printf ("]\n");
    }        
}

void print_status_bands(int status_band [SIZE_B]){
	for (int i = 0; i < SIZE_B; ++i){
  			if(status_band[i]==0)
  				printf("La banda %i tiene estado 'en reposo'.\n", i);
  			else if(status_band[i]==1)
  				printf("La banda %i tiene estado 'en ejecución'.\n", i);
  			else
  				printf("La banda %i tiene estado 'pausado'.\n", i);
  		}
}

void message_help(){
	printf("\t\t\t----Order Band----\n" );
	printf("Manejo del estado de las bandas.  \n");
	printf("Mostrar mensaje de ayuda>>\t ./order_band -h\n");
	printf("Mostrar el estado de las bandas>>\t ./order_band -s\n");
	printf("Pausar banda>>\t ./order_band -p [0-2] \n");
	printf("Reanudar banda>>\t ./order_band -r [0-2] \n");
	printf("Dar mantenimiento a los dispensadores>\t ./order_band -m [0-2] \n");
}
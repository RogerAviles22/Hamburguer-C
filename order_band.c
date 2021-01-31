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

bool sflag = false; 
bool pflag = false; 
bool rflag = false; 
bool mflag = false; 

int preparation_band[SIZE_B][SIZE_C]; //Matriz con las cantidad de cintas y sus respectivos ingredientes
int status_band [SIZE_B]; // 0 en reposo (sin orden), 1 en ejecución, 2 pausado

void message_help();

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

	pos_band = atoi(cvalue);
	printf("%i\n", pos_band);
	if ( pos_band<0 || pos_band>2 ){
		printf("Ingrese un valor númerico del 0-2!\n");
		return -1;
	}

	/*int sockfd;
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
	}*/




	return 0;
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
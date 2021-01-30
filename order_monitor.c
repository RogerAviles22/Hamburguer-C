#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>

#define SIZE_F    10      /* row size */ 
#define SIZE_B    3		  /* band size*/
#define SIZE_C    10      /* column size */ 
#define SIZE_D	  20      /*dispensador size*/

static int preparation_band[SIZE_B][SIZE_C]; //Matriz con las cantidad de cintas y sus respectivos ingredientes
static int status_band [SIZE_B]; // 0 en reposo (sin orden), 1 en ejecución, 2 pausado

static int prepared_burguer_counter[SIZE_B];

static int matrix_orders[SIZE_F][SIZE_C]; //Matriz de ordenes, cada columna va corresponder a un ingrediente
static int status[SIZE_F]; //0 no enviado, 1 en proceso, 2 finalizado
 
/*Llenamos la matriz Banda de Preparacion con ingredientes de valores iniciales fijos de SIZE_D. */
void fill_preparation_bands(int preparation_band[SIZE_B][SIZE_C], int status_band[SIZE_B]){
    int i, j;
 
    for (i = 0; i < SIZE_B; i++){
    	for (int j = 0; j < SIZE_C; j++){
      		preparation_band[i][j]= SIZE_D;
    	}      
    }

    //Inicialmente las bandas estàn en reposo
    for (i = 0; i < SIZE_B; i++)   
        status_band[i]=0;
}

/*Imprime los valores de la matriz preparacion de banda */
void print_preparation_bands (int preparation_band[SIZE_B][SIZE_C] ){
    int i, j;
    int first;
    for (i = 0; i < SIZE_B; i++) {
      printf("Banda #%i:  ", (i+1));
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

/*Imprime los valores de la matriz */
void print_orders (int matrix_orders[SIZE_F][SIZE_C]){
  int i, j;
  int first;

  for (i = 0; i < SIZE_F; i++) {
  	printf("Orden %i: ", i);
    printf ("[");
    first = 1;
    for (j = 0; j < SIZE_C; j++) {
        if (!first)
            printf (",");
        printf ("%x", matrix_orders[i][j]); //IMprime en hexa, como los valores van de 0-5 no hay inconveniente.
        first = 0;
    }
    printf ("]\n");
  }      
}

int validate_contents_bands(int pos_order, int pos_band, int matrix_orders[SIZE_F][SIZE_C], int status_band [SIZE_B], int preparation_band[SIZE_B][SIZE_C]){
  
  if(status_band [SIZE_B] == 1) return -1;

  for (int j = 0; j < SIZE_C; j++){
    if(matrix_orders[pos_order][j] >= preparation_band[pos_band][j] && preparation_band[pos_band][j] ==0 )
      return -1;
  }

  return 1;
}

/*Retorna la cantidad de las ordenes pendientes*/
int pending_items(int matrix_orders[SIZE_F][SIZE_C]){
	int sum,i;
	sum=0;
	for (i = 0; i < SIZE_F; i++) 
		sum=sum+matrix_orders[i][0];
	return(sum);
}

/*Retorna -1 si ocurre un error al sumar los panes
Retorna un i random que representa la orden que aùn no haya sido despachada*/
int next_item (int matrix_orders[SIZE_F][SIZE_C]){
    int i, j;
    int sum;
    
    for (i = 0; i < SIZE_F; i++)
		sum=sum+matrix_orders[i][0];
    if (sum<=0)
        return (-1);
    while(pending_items(matrix_orders)>0){
		i=(rand() % (SIZE_F - 1)); //Va de 0 - 9 
		return(i);	
    }
}

int main(int argc, char *argv[]){
	
	//Declaring process variables.
	int server_sockfd, client_sockfd;
	int server_len ; 
	int rc ; 
	unsigned client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	char buffer[50]; 
	int random_band; //Usado para almacenar valor random banda
	int order;
	struct timespec tim, tim2;
	tim.tv_sec = 0;
	tim.tv_nsec = 500000000L; //medio segundo

	//Remove any old socket and create an unnamed socket for the server.
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htons(INADDR_ANY);
	server_address.sin_port = htons(7734) ; 
	server_len = sizeof(server_address);

	//Avisar al sistema operativo de que hemos abierto un socket y queremos que asocie nuestro programa a dicho socket.
	rc = bind(server_sockfd, (struct sockaddr *) &server_address, server_len);
	printf("RC from bind = %d\n", rc ) ; 
	
	//Create a connection queue and wait for clients
	//Avisar al sistema de que comience a atender dicha conexión de red. 
	rc = listen(server_sockfd, 5);
	printf("RC from listen = %d\n", rc ) ; 

	client_len = sizeof(client_address);

	//Pedir y aceptar las conexiones de clientes al sistema operativo.
	client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_address, &client_len);
	printf("after accept()... client_sockfd = %d\n", client_sockfd) ; 


  	fill_preparation_bands(preparation_band, status_band);
 	print_preparation_bands(preparation_band);
  	srand(time(NULL)); //Para obtener distintos tipos de valores random en cada ejecucion

	//Escribir y recibir datos del cliente
	while(1){
		//memset(matrix_orders,0,sizeof(matrix_orders));	
		memset(matrix_orders,0,sizeof(matrix_orders));	
		rc = read(client_sockfd, &matrix_orders,sizeof(matrix_orders)); //1. Cargamos la matriz orden
		if(rc>0){
			print_orders(matrix_orders);
			read(client_sockfd, &status,sizeof(status)); //1. Cargamos la matriz status				
		}

		/*memset(buffer,0,sizeof(buffer));
		rc = read(client_sockfd, &buffer,sizeof(buffer));*/
		//if(rc == -1) break;

     	if (rc <= 0){
     		/*printf("Espera ordenes\n");
     		sleep(1);*/
     		printf("Apagando máquina\n");
     		sleep(1);
     		printf("¡Adiòs!\n");
     		sleep(1);
     		break;
     	} 

     	while(pending_items(matrix_orders)>0){

		    //nanosleep(&tim , &tim2); 
		    order=next_item(matrix_orders);

		    // -- Iniciamos evaluacion de los estados 0 no enviado, 1 en proceso, 2 finalizado
		    if (status[order]==2) {
		      while(status[order++]!=2);
		    }
		    
		    //memset(buffer,0,sizeof(buffer)); //Llenamos con ceros el buffer. 
		      //Envìa a order_monitor la orden
		      //Escribir y recibir datos
		    if (status[order]==0) {
		      status[order]=1;
		      //Almacenamos el valor a escribir en el socket. 
		      /*sprintf(buffer,"Orden %d Ingredientes %d-%d-%d-%d-%d-%d-%d-%d-%d-%d",i,matrix_orders[i][0],matrix_orders[i][1],matrix_orders[i][2],matrix_orders[i][3],matrix_orders[i][4],matrix_orders[i][5],matrix_orders[i][6],matrix_orders[i][7],matrix_orders[i][8],matrix_orders[i][9]);
		      rc = write(sockfd, &buffer, strlen(buffer));*/
		      continue;   //Forza a continuar el sgte while
		    }   

		    //Si el estado de la orden es *en proceso*
		    if (status[order]==1) {
		    	int b_vacios_dispensadores = 0; //En caso que quede dentro del bucle validate_contents_bands, se sospecha dispensadores vacios
				do{
					random_band = (rand() % SIZE_B);
					b_vacios_dispensadores +=1;
				}while(validate_contents_bands(order, random_band, matrix_orders, status_band, preparation_band)<0 && b_vacios_dispensadores<5);

				if(b_vacios_dispensadores >=5){
					printf("╔══════════════════════════╗\n");
					printf("║  ALERTA  DISPENSADOR %i   ║\n", random_band);
					printf("╚══════════════════════════╝\n");
					break;
				}

				status_band[random_band] = 1;
				//Restamos los ingredientes
				for (int m = 0; m < SIZE_C; m++)
					preparation_band[random_band][m] -= matrix_orders[order][m];


				prepared_burguer_counter[random_band]+=1; //Aumentamos el valor de hamburguesas preparados
				printf(buffer,"La banda %i prepara orden %i",(random_band+1), order);
				//nanosleep(&tim , &tim2); 
				//rc = write(sockfd, &buffer, strlen(buffer));

				//Setea todo los valores a 0 de la orden i.
				for (int j = 0; j < SIZE_C; j++){
					matrix_orders[order][j]=0;
					status[order]=2;
				}

				status_band[random_band] = 0;     
		    }
		      // -- Fin de la evaluacion --    
  		}
  		printf("[\t--Estados de la bandas---]\n");
		print_preparation_bands(preparation_band);
		printf("Hamburguesas preparadas por banda\n" );
		for (int m = 0; m < SIZE_B; m++)
			printf("La banda %i preparo %i hamburguesas.\n", (m+1), (prepared_burguer_counter[m]));


     	
		/*if(strstr(buffer, "banda") != NULL){
			printf("╠══ %s ,rc=%d ══╣\n",buffer,rc); 
		}
     	else if (strstr(buffer, "Orden") != NULL) {
			printf("■ %s ,rc=%d\n",buffer,rc); 		    
		}else{
			printf("%s ,rc=%d\n", buffer, rc);
		}*/
	
	}

	printf("server exiting\n");

	close(client_sockfd);
	return 0;
}

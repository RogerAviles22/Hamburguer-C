#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define SIZE_F    10      /* row size */ 
#define SIZE_B    3  /* band size*/
#define SIZE_C    10      /* column size */ 
#define SIZE_D	  10      /*dispensador size*/

int preparation_band[SIZE_B][SIZE_C]; //Matriz con las cantidad de cintas y sus respectivos ingredientes
int status_band [SIZE_B]; // 0 en reposo (sin orden), 1 en ejecución, 2 pausado

int prepared_burguer_counter[SIZE_B];

int matrix_orders[SIZE_F][SIZE_C]; //Matriz de ordenes, cada columna va corresponder a un ingrediente
int status[SIZE_F]; //0 no enviado, 1 en proceso, 2 finalizado, 3 encolado

pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;
 
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

/*SI LA BANDA ESTÀ OCUPADA o SI LOS INGREDIENTES COINCIDEN Y SU VALOR ES 0, ENTONCES RETORNA -1*/
int validate_contents_bands(int pos_order, int pos_band, int matrix_orders[SIZE_F][SIZE_C], int status_band [SIZE_B], int preparation_band[SIZE_B][SIZE_C]){
  
  /*if(status_band [SIZE_B] == 1) return -1;

  for (int j = 0; j < SIZE_C; j++){
    if(matrix_orders[pos_order][j] >= preparation_band[pos_band][j] && preparation_band[pos_band][j] ==0 )
      return -1;
  }
  return 1;*/
	if(status_band [SIZE_B] == 0){ 
		for (int j = 0; j < SIZE_C; j++){
		    if(matrix_orders[pos_order][j] >= preparation_band[pos_band][j] && preparation_band[pos_band][j] ==0 )
		    	return -1;
			}
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
Retorna un i drepresenta la orden que aùn no haya sido despachada*/
int next_item (int matrix_orders[SIZE_F][SIZE_C]){
    int i;
    int sum;

    if(pending_items(matrix_orders)<=0)
    	return -1;
    
    for (i = 0; i < SIZE_F; i++){
		sum=sum+matrix_orders[i][0];
		if(status[i]==0 && sum>0 ){	
			status[i]==1; 		
			return i;
		}
    }
}

void *band_thread(void *arg){
	int order ;
	int num_band= *((int *)arg); //Num de banda dispensadora
	struct timespec tim, tim2;
	tim.tv_sec = 0;
	tim.tv_nsec = 500000000L; //medio segundo

	while(pending_items(matrix_orders)>0){

			pthread_mutex_lock(&mutex);
		    order=next_item(matrix_orders);
		    pthread_mutex_unlock(&mutex);

		    if (order<0)
		    	break;		    

		    // -- Iniciamos evaluacion de los estados 0 no enviado, 1 en proceso, 2 finalizado
		    if (status[order]==2) 
		    	while(status[order++]!=2);

		    if (status[order]==0) 
				status[order]=1;		       

		    //Si el estado de la orden es *en proceso*
		    if (status[order]==1) {
		    	int b_vacios_dispensadores = 0; //En caso que quede dentro del bucle validate_contents_bands, se SOSPECHA dispensadores vacios
				do{
					b_vacios_dispensadores +=1;
				}while(validate_contents_bands(order, num_band, matrix_orders, status_band, preparation_band)<0 && b_vacios_dispensadores<5);

				if(b_vacios_dispensadores >=5){
					printf("╔══════════════════════════╗\n");
					printf("║	ALERTA  BANDA %i   ║\n", num_band);
					printf("╚══════════════════════════╝\n");
					break;
					//continue;
				}else{
					//pthread_mutex_lock(&mutex);
					status_band[num_band] = 1;
			    	//pthread_mutex_unlock(&mutex);

					//Restamos los ingredientes
					for (int i = 0; i < SIZE_C; i++)
						preparation_band[num_band][i] -= matrix_orders[order][i];

			    	//pthread_mutex_lock(&mutex);
					prepared_burguer_counter[num_band]+=1; //Aumentamos el valor de hamburguesas preparados
			    	//pthread_mutex_unlock(&mutex);
					
					printf("La banda %i prepara orden %i\n",(num_band+1), order);
					nanosleep(&tim , &tim2); 

					for (int j = 0; j < SIZE_C; j++){
						matrix_orders[order][j]=0;
						status[order]=2;
					}
					status_band[num_band] = 0;    
				}		    	 
		    }
		      // -- Fin de la evaluacion --    
  		}
  		return NULL;
}



int main(int argc, char *argv[]){
	//Declaring process variables.
	int server_sockfd, client_sockfd;
	int server_len ; 
	int rc ; 
	unsigned client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	int order, vendidos = 10;

	//Hilos como Bandas
	pthread_t tid_bands1; 
	pthread_t tid_bands2;  
	pthread_t tid_bands3;

	//Remove any old socket and create an unnamed socket for the server.
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htons(INADDR_ANY);
	server_address.sin_port = htons(7734) ; 
	server_len = sizeof(server_address);

	//Avisar al sistema operativo de que hemos abierto un socket y queremos que asocie nuestro programa a dicho socket.
	rc = bind(server_sockfd, (struct sockaddr *) &server_address, server_len);
	printf("RC from bind = %d\n", rc ) ; 
	
	//Create a connection queue and wait for clients. Avisar al sistema de que comience a atender dicha conexión de red. 	
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

		memset(matrix_orders,0,sizeof(matrix_orders));	
		rc = read(client_sockfd, &matrix_orders,sizeof(matrix_orders)); //1. Cargamos la matriz orden
		if(rc>0){
			printf("╠══ Turno %i ══╣\n", vendidos);
			read(client_sockfd, &status,sizeof(status)); //2. Cargamos la matriz status		
			vendidos+=10;		
		}

     	if (rc <= 0){
     		printf("Apagando máquina\n");
     		sleep(1);
     		printf("¡Adiós!\n");
     		sleep(1);
     		break;
     	} 

     	int pos0 =0, pos1 =1, pos2 =2;

     	pthread_create(&tid_bands1, NULL, band_thread, &pos0);
     	pthread_create(&tid_bands2, NULL, band_thread, &pos1);
     	pthread_create(&tid_bands3, NULL, band_thread, &pos2);

     	//printf("HILOS CREADOS!\n");

		pthread_join(tid_bands1, NULL);
		pthread_join(tid_bands2, NULL);
		pthread_join(tid_bands3, NULL);

     	/*for (int i = 0; i < SIZE_B; i++){
     		printf("Enviado banda %i\n", i );
     		pthread_create(&tid_bands[i], NULL, band_thread, &i);
     	}
     	
     	for (int i = 0; i < SIZE_B; i++){
     		pthread_join(tid_bands[i], NULL);
     	}*/

     	
  		printf("[\t--Estados de la bandas---]\n");
		print_preparation_bands(preparation_band);
		printf("Hamburguesas preparadas por banda\n" );
		for (int i = 0; i < SIZE_B; i++)
			printf("La banda %i preparo %i hamburguesas.\n", (i+1), (prepared_burguer_counter[i]));		
     	
		/*if(strstr(buffer, "banda") != NULL){
			printf("╠══ %s ,rc=%d ══╣\n",buffer,rc); 
		}*/
	
	}

	printf("server exiting\n");

	close(client_sockfd);
	return 0;
}

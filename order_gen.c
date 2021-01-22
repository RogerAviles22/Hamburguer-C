/*
 * order_generator.c
 *
 */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SIZE_F    10      /* row size */ 
#define SIZE_C    10      /* column size */ 
#define SIZE_B    3		  /* band size*/
#define SIZE_D	  30      /*dispensador size*/

static int matrix_orders[SIZE_F][SIZE_C]; //Matriz de ordenes, cada columna va corresponder a un ingrediente
static int status[SIZE_F]; //0 no enviado, 1 en proceso, 2 finalizado

char list_ingredients [SIZE_C][20] = {"Bread", "Meat", "Tomato", "Onion", "Ham", "Egg", "Lettuce", "Mayo", "French fries"}; 

static int preparation_band[SIZE_B][SIZE_C]; //Matriz con las cantidad de cintas y sus respectivos ingredientes
static int status_band [SIZE_B]; // 0 en reposo (sin orden), 1 en ejecución, 2 pausado
static int prepared_burguer_counter[SIZE_B];

/*Imprime los valores de la matriz */
void print_orders (int matrix_orders[SIZE_F][SIZE_C]){

    int i, j;
    int first;

    for (i = 0; i < SIZE_F; i++) {
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

/*Llenamos la matriz Orden con ingredientes de valores random. */
void fill_orders (int matrix_orders[SIZE_F][SIZE_C],int status[SIZE_F]){
    int i, j;
    int first;

	//Considerar la forma de generar la cantidad de ingredientes para cada orden    
    for (i = 0; i < SIZE_F; i++){
      matrix_orders[i][0]= 2;    //Bread - 2 Obligatorio
      matrix_orders[i][1]= (rand() % (3-1) + 1); //Meat - Obligatorio de 1 a 2
      matrix_orders[i][2]= (rand() % 2) ;    //tomato De 0-1
      matrix_orders[i][3]= (rand() % 2) ;    //cebolla De 0-1
      matrix_orders[i][4]= (rand() % 3) ;    //Ham De 0-2
      matrix_orders[i][5]= (rand() % 3) ;    //Egg De 0-2
      matrix_orders[i][6]= (rand() % 3) ;    //Bacon De 0-2
      matrix_orders[i][7]= (rand() % 2) ;    //Lettuce De 0-1
      matrix_orders[i][8]= (rand() % 2) ;    //Mayo De 0-1
      matrix_orders[i][9]= (rand() % 3) ;    //Frech fries De 0-2
    }

    //Seteamos el estado de las bandas segun la orden procesada
    for (i = 0; i < SIZE_F; i++)   
        status[i]=0;
}

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

int main (int argc, char *argv[]){
   int i;
   struct timespec tim, tim2;
   tim.tv_sec = 0;
   tim.tv_nsec = 500000000L; //medio segundo
   char buffer[50]; 
   int sockfd;
   int len, rc ;
   struct sockaddr_in address;
   int result;
   int random_band; //Usado para almacenar valor random banda

   // -- Inicio de la conexion -- 
    //Create socket for client.
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
	result = connect(sockfd, (struct sockaddr *)&address, len);

	if(result == -1){
		perror("Error has occurred");
		exit(-1);
	}
  // -- Fin de la conexion --

  	srand(time(NULL)); //Para obtener distintos tipos de valores random en cada ejecucion

  	printf("\t\t---List of Ingredients---\n");
  	for(int k=0; k < SIZE_C; k++ )
  		printf("%s ", list_ingredients[k]);
  	printf("\n");

  	// -- Inicio creacion de las 10 Ordenes --
    fill_orders(matrix_orders,status); //Llenamos las ordenes
    print_orders (matrix_orders); //Mostramos los datos


  	fill_preparation_bands(preparation_band, status_band);
  	//print_preparation_bands(preparation_band);

    printf("%d",pending_items(matrix_orders));
    //Muestra los datos cada 0.5 seg mientras haya ordenes pendientes
    while(pending_items(matrix_orders)>0){
		nanosleep(&tim , &tim2); 
		i=next_item(matrix_orders);

		// -- Iniciamos evaluacion de los estados 0 no enviado, 1 en proceso, 2 finalizado

	    if (status[i]==2) {
	   		while(status[i++]!=2);
	    }
	    memset(buffer,0,sizeof(buffer));
	    //Envìa a order_monitor la orden
		if (status[i]==0) {
			status[i]=1;
			sprintf(buffer,"Orden %d Ingredientes %d-%d-%d-%d-%d-%d-%d-%d-%d-%d",i,matrix_orders[i][0],matrix_orders[i][1],matrix_orders[i][2],matrix_orders[i][3],matrix_orders[i][4],matrix_orders[i][5],matrix_orders[i][6],matrix_orders[i][7],matrix_orders[i][8],matrix_orders[i][9]);
			rc = write(sockfd, &buffer, strlen(buffer));
			continue;		
		}		

		//Su el estado de la orden es en proceso
		if (status[i]==1) {		    
			random_band = (rand() % SIZE_B);
			//Restamos los ingredientes
			for (int m = 0; m < SIZE_C; m++)
			 	preparation_band[random_band][m] -= matrix_orders[i][m];
			
			prepared_burguer_counter[random_band]+=1; //Aumentamos el valor de hamburguesas preparados
			//Setea todo los valores a 0 de la orden i.
			for (int j = 0; j < SIZE_C; j++){
		    	matrix_orders[i][j]=0;
				status[i]=2;
			}
    	}

    	// -- Fin de la evaluacion --    
  	}

  	printf("--Resultados finales---\n");
  	print_preparation_bands(preparation_band);
  	printf("Hamburguesas preparadas\n" );
  	for (int m = 0; m < SIZE_B; m++)
  		printf("La banda %i preparo %i hamburguesas.\n", (m+1), (prepared_burguer_counter[m]));


    return 0;
}

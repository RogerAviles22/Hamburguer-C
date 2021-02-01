/*
 * order_generator.c
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SIZE_F    10      /* row size */ 
#define SIZE_C    10      /* column size */ 

int matrix_orders[SIZE_F][SIZE_C]; //Matriz de ordenes, cada columna va corresponder a un ingrediente
int status[SIZE_F]; //0 no enviado, 1 en proceso, 2 finalizado

char list_ingredients [SIZE_C][20] = {"Bread", "Meat", "Tomato", "Onion", "Ham", "Egg", "Lettuce", "Mayo", "Baccon" ,"French-Fries"}; 

void mostrarMensajeAudiencia(){
  printf("\t¡Bienvenidos a BURGUER-MAKING MACHINE V2!\n");
  printf("\t\t\t_/¨¨¨¨¨\\_\n");
  printf("\t\t\t~~~~~~~~~\n");
  printf("\t\t\t█████████\n");
  printf("\t\t\t▒▒▒▒▒▒▒▒▒\n");
  printf("\t\t\t\\_______/\n");
  printf("Por cuestiones de distanciamiento social, se aceptará el ingreso de 10 usuarios. \n");
  printf("Restricción: 1 pedido x usuario. \n");
}

/*Imprime la lista de ingredientes.*/
void print_list_ingredients(){
  printf("\t\t---List of Ingredients---\n");
  for(int k=0; k < SIZE_C; k++ )
    printf("%s ", list_ingredients[k]);
  printf("\n");
}


/*Imprime los valores de la matriz */
void print_orders (int matrix_orders[SIZE_F][SIZE_C]){
  int i, j;
  int first;
  struct timespec tim, tim2;
  tim.tv_sec = 0;
  tim.tv_nsec = 500000000L; //medio segundo

  for (i = 0; i < SIZE_F; i++) {
  	printf("Orden %i: ", i);
    printf ("[");
    first = 1;
    for (j = 0; j < SIZE_C; j++) {
        if (!first)
            printf (",");
        printf ("%x", matrix_orders[i][j]); //Imprime en hexa, como los valores van de 0-5 no hay inconveniente.
        first = 0;
    }
    printf ("]\n");
    nanosleep(&tim , &tim2);  
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
      matrix_orders[i][9]= (rand() % 3) ;    //French fries De 0-2
    }

    //Seteamos el estado de las bandas segun la orden procesada
    for (i = 0; i < SIZE_F; i++)   
        status[i]=0;
}

int main (){
  
	char buffer[50]; 
	int sockfd;
	int len, rc ;
	struct sockaddr_in address;
	int result;
	//int random_band; //Usado para almacenar valor random banda
  int opt_news_orders=0;
  int opt_action=5;

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
  // -- Fin de la conexion --

  srand(time(NULL)); //Para obtener distintos tipos de valores random en cada ejecucion

  mostrarMensajeAudiencia();

  print_list_ingredients(); 
  // -- Inicio creacion de las 10 Ordenes --
  memset(matrix_orders,0,sizeof(matrix_orders)); //Llenamos con ceros el buffer.
  fill_orders(matrix_orders,status); //Llenamos las nuevas ordenes
  print_orders (matrix_orders); //Mostramos los datos

do{
  if(opt_news_orders==1){ //Si la opción es 1, crea nuevas ordenes.
      fill_orders(matrix_orders,status); //Llenamos las nuevas ordenes
      print_orders (matrix_orders); //Mostramos los datos
  }

  write(sockfd, &opt_action, sizeof(opt_action)); //0. Enviamos la action opt

  write(sockfd, &matrix_orders, sizeof(matrix_orders)); //1. Enviamos la matriz_orders
  write(sockfd, &status, sizeof(status)); //2. Enviamos la matriz_status
  //memset(buffer,0,sizeof(buffer));
  
  //rc =  read(sockfd, &buffer,sizeof(buffer) ); //3. Recibe orden despachada

  printf("¿Ingresar 10 nuevos pedidos?\nSí = 1\nNo = 0\nRespuesta : ");
  scanf("%i",&opt_news_orders); //Leyendo el número solicitado
}while(opt_news_orders==1);

  printf("Vuelva pronto!\n");
  close(result); //Cerramos la conexion con el socket
  return 0;
}

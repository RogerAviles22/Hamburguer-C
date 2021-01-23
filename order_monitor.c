#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

 

int main(int argc, char *argv[]){
	
	//Declaring process variables.
	int server_sockfd, client_sockfd;
	int server_len ; 
	int rc ; 
	unsigned client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	char buffer[50]; 

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

	//Escribir y recibir datos del cliente
	while(1){
		memset(buffer,0,sizeof(buffer));
		rc = read(client_sockfd, &buffer,sizeof(buffer));
		//if(rc == -1) break;
     	if (rc <= 0){
     		printf("Espera ordenes\n");
     		sleep(1);
     	} 
		printf("[Data = %s rc=%d]\n",buffer,rc); 
		/*sleep(1);
		printf("%d\n",rc);	*/	
	
	}

	printf("server exiting\n");

	close(client_sockfd);
	return 0;
}

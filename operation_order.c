#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

bool validate_orders_band(status_band [SIZE_B]){
	if (status_band[SIZE_B] == 0 )
		return true;
	return false;
}

bool decrement_bands_order(matrix_orders[SIZE_F][SIZE_C], preparation_band [SIZE_B][SIZE_C]){
	if(validate_orders_band){
		for (int i = 0; i < count; ++i)
		{
			/* code */
		}
	}
}


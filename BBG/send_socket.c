
/*******************************************************************************************************
*
* UNIVERSITY OF COLORADO BOULDER
*
* @file main.c
* Socketc : Cleint connection for API  requests
* @author Kiran Hegde and Gautham 
* @date  4/29/2018
* @tools vim editor
*
********************************************************************************************************/
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "socket.h"
#include <arpa/inet.h>
#include <stdint.h>
#include <netinet/in.h>
#include "log.h"

/* define port number */

/********************************************************************************************************
*
* @name main
* @brief main function
*
* Opens a socket and binds it to port. connects to server and sends data.
* log the incoming data from server
*
* @param None
*
* @return zero on successful execution, otherwise error code
*
********************************************************************************************************/

int main()
{

    int client, sock, read_sock;
	int repeat=0;
    struct sockaddr_in address;
    int len = sizeof(address);
    uint32_t opt, recv;

	/* open socket */
	while(!repeat)
	{	
        if((client = socket(AF_INET, SOCK_STREAM, 0))<0)
        {
                printf("Client creation failed\n");
                exit(0);
        }
	
		address.sin_family = AF_INET;
		address.sin_port = htons(PORT);
	
		/* converts IP address to proper format */
		if((inet_pton(AF_INET, "127.0.0.1", &address.sin_addr))<=0)
		{
			printf("Addr error\n");
		}
	
		if((connect(client, (struct sockaddr *)&address, sizeof(address)))<0)
			perror("connect:");
		
		

		printf("Choose Any option\n");
		printf("1. Read Relay 1 status \n");
		printf("2. Read Relay 2 Status\n");
		printf("3. Get Gesture Sensor ID\n");
		printf("4. Enable Gesture Sensor\n");
		printf("5. Disable Gesture Sensor\n");
		printf("6. Increase Gain of Input\n");
		printf("7. Perform Gesture of turning on both devices \n");
		printf("8. Perform Gesture of turning off both devices \n");
		
		scanf("%d", &opt);
			

		send(client, &opt, sizeof(opt), 0);
			
		read(client, &recv, sizeof(recv));
	
		if(opt==1){
			if(recv == 0)
				printf("RELAY 1 is switched OFF\n"); 
			else
				printf("RELAY 1 is switched ON\n"); 
		}
		else if(opt ==2){
			if(recv == 0)
				printf("RELAY 2 is switched OFF\n"); 
			else
				printf("RELAY 2 is switched ON\n"); 
		}

		else if(opt ==3)
			printf("Gesture Sensor ID : %d \n",recv);
		
		else if(opt ==4){
			if(recv == 0)
				printf("Gesture Sensor Enable Failed\n"); 
			else
				printf("Gesture Sensor Enable Done\n"); 
		}
		else if(opt ==5){
			if(recv == 0)
				printf("Gesture Sensor disable failed\n"); 
			else
				printf("Gesture Sensor Disable done\n"); 
		}

		else if(opt ==6){
			if(recv == 0)
				printf("Increase Gain : Not performed\n"); 
			else
				printf("Increase Gain : Performed\n"); 
		}	

		else if(opt ==7){
			if(recv == 0)
				printf("Both Sensors not turned ON\n"); 
			else
				printf("Both Sensors turned ON\n"); 
		}				

		else if(opt ==8){
			if(recv == 0)
				printf("Both Sensors not turned OFF\n"); 
			else
				printf("Both Sensors turned OFF\n"); 
		}
			

		close(client);
	}
	//close(client);
	shutdown(client, 2);
	return 0;
}
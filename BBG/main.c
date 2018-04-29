#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "uart.h"
#include "log.h"
#include <signal.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include "socket.h"


FILE *fp_log;
mqd_t socket_q,sock_ans_q;
mqd_t hb_comm_q,hb_sock_q,hb_log_q;

static void* communication(void *arg);
static void* logger(void *arg);
static void* socket_cli(void *arg);
static void* decision(void *arg);

pthread_t comm_thread,logger_thread,socket_thread,decision_thread;

pthread_mutex_t log_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t uart_lock=PTHREAD_MUTEX_INITIALIZER;



int client_call;
char *filename ;
/* file descriptor for uart device*/
int file;
sig_atomic_t logger_end,comm_thread_end,socket_end, decision_end, kill_process;

sig_atomic_t hb_comm,hb_socket,hb_logger;

void signal_handler()
{
	logger_end =1;
	comm_thread_end=1;
	socket_end = 1;
	decision_end  =1;
	kill_process =1;
	if(!fp_log)
	{
		fflush(fp_log);
        fclose(fp_log);
		fp_log = NULL;
	}
	close(file);
	
	mq_close(log_q);
	mq_unlink(LOG_QUEUE);

	mq_close(socket_q);
	mq_unlink(SOCKET_QUEUE);

	mq_close(sock_ans_q);
	mq_unlink(SOCK_REPLY_QUEUE);

	pthread_cancel(comm_thread);
	pthread_join(comm_thread, NULL);
	
	pthread_cancel(logger_thread);
	pthread_join(logger_thread, NULL);

	pthread_cancel(socket_thread);
	pthread_join(socket_thread, NULL);
	exit(1);
}




int  main(int argc, char *argv[]){


	//uart_init();
	int count =0 ;
	char *str1 = "gautham";
	filename = argv[1];

	uart_init();

	/* VALUES FOR hearbeat*/

	hb_comm = 0,hb_socket =0 ,hb_logger=0;

	signal(SIGINT, signal_handler);

	/* message quque for logging*/
	kill_process = 0;

	if(!argv[1])
	{
		printf("No Filename entered \n");
		return -1;
	}

	struct mq_attr attr_log;
	attr_log.mq_maxmsg = 5;
    attr_log.mq_msgsize = sizeof(Logger_t);

	mq_unlink(LOG_QUEUE);
	mq_unlink(SOCKET_QUEUE);
	mq_unlink(SOCK_REPLY_QUEUE);

	if((log_q = mq_open(LOG_QUEUE, O_RDWR | O_CREAT, 0666, &attr_log))==-1)
    	{
            perror("LOG QUEUE: ");
            exit(1);
   	 }

    struct mq_attr attr;
    attr.mq_maxmsg = 5;
    attr.mq_msgsize = sizeof(int);
    
    
    if((socket_q = mq_open(SOCKET_QUEUE, O_RDWR | O_CREAT | O_NONBLOCK, 0666, &attr))==-1)
    {
                perror("Socket QUEUE: ");
                exit(1);
    }

	
    if((sock_ans_q = mq_open(SOCK_REPLY_QUEUE, O_RDWR | O_CREAT, 0666, &attr))==-1)
    {
                perror("Socket_Reply QUEUE: ");
                exit(1);
    } 

	if(pthread_create(&comm_thread,NULL,communication,(void*)NULL))
	{
		printf("Could not create communication thread\n");
		exit(1);
	}	
	
	
	if(pthread_create(&logger_thread,NULL,logger,(void*)NULL))
	{
		printf("Could not create LOGGER thread\n");
		exit(1);
	} 
	
	if(pthread_create(&socket_thread,NULL,socket_cli,(void*)NULL))
	{
		printf("Could not create SOCKET thread\n");
		exit(1);
	} 
	


	LOG(LOG_LEVEL_INIT,LOG_SOURCE_MAIN,"BBG_Main_Task Initialised: Threads Created",NULL,NULL);

	uint8_t val_rec;
	uint8_t hbcomm_count=0,hbsock_count=0,hblog_count=0;
	
	
	while(!kill_process)
	{	
		if(hb_comm == 0){
			if(hbcomm_count++ >20)
				LOG(LOG_LEVEL_HEARTBEAT,LOG_SOURCE_MAIN ,"NO HEARTBEAT FROM COMMUNICATION TASK " ,NULL,NULL);
			}
		else
		{
			LOG(LOG_LEVEL_HEARTBEAT,LOG_SOURCE_MAIN,"ALIVE FROM COMMUNICATION TASK",NULL,NULL);
			hb_comm =0;
			hbcomm_count = 0;
		}	

		if(hb_socket == 0){
			if(hbsock_count++ >20)
				LOG(LOG_LEVEL_HEARTBEAT,LOG_SOURCE_MAIN ,"NO HEARTBEAT FROM SOCKET TASK " ,NULL,NULL);
			}
		else
		{
			LOG(LOG_LEVEL_HEARTBEAT,LOG_SOURCE_MAIN,"ALIVE FROM SOCKET TASK",NULL,NULL);
			hb_socket=0;
			hbsock_count = 0;
		}

		if(hb_logger == 0){
			if(hblog_count++ >20)
				LOG(LOG_LEVEL_HEARTBEAT,LOG_SOURCE_MAIN ,"NO HEARTBEAT FROM LOGGER TASK " ,NULL,NULL);
			}
		else
		{
			LOG(LOG_LEVEL_HEARTBEAT,LOG_SOURCE_MAIN,"ALIVE FROM LOGGER TASK",NULL,NULL);
			hb_logger=0;
			hblog_count = 0;
		}
			

		sleep(1);
	}
	
	pthread_join(comm_thread,NULL);
	pthread_join(logger_thread,NULL);
   	pthread_join(socket_thread,NULL);


}

static void* communication(void *arg){
	//struct thread_info *tinfo = (struct thread_info*)arg;	
	int count=0;
	comm_thread_end = 0;
	uint8_t val_hb = 1;
	char recv;
	int info=0,i;
	uint8_t val;
	uint32_t client_call= 0;
	struct timespec tm;
	uint32_t reply;
	uint8_t  tiva_hb,val_from_tiva,proceed=1;
	uint32_t checksum_rec=0;
	Logger_t log,log_send;
	
	LOG(LOG_LEVEL_INIT,LOG_SOURCE_COMM,"BBG_COMMUNICAION_Task Initialised",NULL,NULL);
	memset(&log,'\0',sizeof(Logger_t));
	while(!comm_thread_end)
	{
		
		hb_comm =1;
		
		if(proceed == 1)
		{	
			printf("blocked\n");
			count = read(file,&log,sizeof(log));

			//if(count ==0)
			//	proceed =0;
			printf("first read%d\n",count);
			printf("nnn");
			//checksum_rec = log.timestamp ^ log.log_level ^ log.log_source ;
			//checksum_rec ^= log.log_level;
			//checksum_rec ^= log.log_source;
			/*
			for(i= 0; i<MSG_SIZE;i++) 
				checksum_rec ^= log.message[i];
			*/

			//if (checksum_rec == log.checksum)
			//	printf("Perfect Checksum\n");	

			//printf(" made : %d \n",checksum_rec);
		       //printf("original : %d \n", log.checksum);
			printf("tiva log Heartbeat %d\n",log.log_level);
			if(log.log_level == LOG_LEVEL_HEARTBEAT || log.log_level == LOG_LEVEL_INIT ||  log.log_level==LOG_LEVEL_INFO )
			{
				tiva_hb = 0x4d;
				usleep(1000);
				pthread_mutex_lock(&uart_lock);
				count = write(file,&tiva_hb,sizeof(tiva_hb));
				pthread_mutex_unlock(&uart_lock);
				printf("hi\n");
				tiva_hb = 0;
			}


			if((mq_send(log_q, &log, sizeof(log),0))==-1)
	        {
	            printf("cant send message to process1 and returned %d\n", errno);
	        } 

			/*
			if(log.log_level == 17){
				tiva_hb = 1;
				count = write(file,&tiva_hb,sizeof(tiva_hb));
				printf("Write to TIVA%d\n",count);	
			}
			*/

	        
		

	        

	        /* receive socket client request */
		
			/*
			sleep(1);
			if((mq_receive(socket_q, &info, sizeof(info), 0))==-1)
			{
				printf("No Client call Connection\n");
			}
			

			else
			{
				printf("val to tiva..\n");
				if (info >0){
					val = (uint8_t)info;
					printf("Value info %d\n",info);
					printf("val sent to tiva %d\n",info);
					switch(info)
					{

						case 1 : val = 0x01;
						 	break;
						case 2 : val = 0x02;
							break;
						case 3: val = 0x03;
					       		break;
						case 4 : val = 0x04;
					 		break;
						case 5 : val = 0x05;
							break;
						case 6 : val = 0x06;
							break;
						case 7 : val = 0x07;
							break;
						case 8 :val = 0x08;
				       			break;
						default: ;
				 	}

					count = 0;
					count  = write(file,&val,sizeof(val));
					printf("Bytes to tiva : %d\n",count);
					info =0;
				}
			}
			*/

			

			client_call = log.log_source;
			if(client_call == 18){
				reply = log.value;
				printf("reply : %d\n",reply);
				printf("valueee\n\n\n");

				if((mq_send(sock_ans_q,&reply,sizeof(reply),0))==-1)
				{
					printf("Cannot write client call answer\n");
				}
			}
			

		
		}
		//sleep(1);

		
	}

	//close(file);
	//pthread_exit((void*)0);	
}


static void* logger(void *arg){	

	//char *filename = argv[1];
	Logger_t log;
	uint8_t val_hb = 3;
	fp_log = fopen(filename, "wb");

	logger_end =0 ;
	
	if(!fp_log)
	{
		printf("File can't be opened\n");
                exit(1);
	}  

	fprintf(fp_log, "Timestamp\t");
    fprintf(fp_log, "LOG_LEVEL\t");
    fprintf(fp_log, "LOG_SOURCE\t");
    fprintf(fp_log, "Value\t");
    fprintf(fp_log, "Message\r\n");
    fflush(fp_log);
    fclose(fp_log);
    fp_log = NULL;

    LOG(LOG_LEVEL_INIT,LOG_SOURCE_LOGGER,"BBG_Logger_Task Initialised",NULL,NULL);
    while(!logger_end)
    {


    	hb_logger =1;

    	fp_log = fopen(filename, "a");
        	if(!fp_log)
                	exit(1);
		
		if((mq_receive(log_q, &log, sizeof(Logger_t), 0))==-1)
		{
			printf("Din't receive message from commn thread and returned %d\n", errno);
			exit(1);
		}
		    fprintf(fp_log, "%d\t\t",log.timestamp);
        	fprintf(fp_log, "%d\t\t",log.log_level);
        	fprintf(fp_log, "%d\t\t", log.log_source);	
        	fprintf(fp_log, "%d\t",log.value);
        	fprintf(fp_log, "%s\n", log.message);
		
			fflush(fp_log);
			fclose(fp_log);
			fp_log = NULL;


    }

}


static void* socket_cli(void *arg)
{
	int sock;
	socket_end = 0;
	uint8_t val_hb = 2;
	struct sockaddr_in address;
	int len = sizeof(address);
	int count =0,option =1;
	uint32_t request,info,reply;
	uint8_t val;
	LOG(LOG_LEVEL_INIT,LOG_SOURCE_SERVER,"BBG_Server_Task Initialised",NULL,NULL);
	int server,read_sock;

	if((server = socket(AF_INET, SOCK_STREAM, 0))==0)
	{
		printf("server creation failed");
		exit(1);
	} 

	/* set socket options */
	if(setsockopt(server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option)))
	{
		printf("Can't set socket");
		exit(1);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	
	/* bind the socket to port mentioned */
	if((bind(server, (struct sockaddr *)&address, sizeof(address)))<0)
	{
		printf("Port binding failed");
		exit(NULL);
	}

	if(listen(server, 3) < 0)
	{
			//LOG(LOG_LEVEL_ERROR, SOCK_TASK, "Can't listen", NULL);i
			perror("Listen: \n");
	}


	LOG(LOG_LEVEL_INIT,LOG_SOURCE_SERVER,"BBG_Server_Task Initialised",NULL,NULL);
	while(!socket_end)
	{
		

		hb_socket=1;

		sock = accept(server, (struct sockaddr *)&address, (socklen_t *)&len);
		if(sock < 0)
		{
			//LOG(ERROR, SOCK_TASK, "Can't accept connection", NULL);
			perror("Accept: \n");
		}
		
		
		read_sock = read(sock, &request, sizeof(request));

		printf("Client Value %d\n",request);

		switch(request)
					{

						case 1 : val = 0x01;
						 	break;
						case 2 : val = 0x02;
							break;
						case 3: val = 0x03;
					       		break;
						case 4 : val = 0x04;
					 		break;
						case 5 : val = 0x05;
							break;
						case 6 : val = 0x06;
							break;
						case 7 : val = 0x07;
							break;
						case 8 : val = 0x08;
				       			break;
						default: ;
				 	}


		printf("val set conf : %d", val);
		pthread_mutex_lock(&uart_lock);

		count = write(file,&val,sizeof(val));
		//printf("count : %d",count);

		pthread_mutex_unlock(&uart_lock);




/*
		if((mq_send(socket_q, &info, sizeof(info),0))== -1)
        {
            printf("cant send message to comm thread and returned %d\n", errno);
      	 }

*/

       	if((mq_receive(sock_ans_q, &reply, sizeof(reply), 0))==-1)
		{
			printf("Din't receive message from commn thread and returned %d\n", errno);
			exit(1);
		}

		printf("mq_rec socket %d",reply);
		if(reply>0)
			reply = 1;

		send(sock,&reply,sizeof(reply),0);
		printf("reply to client %d\n\n\n\n",reply);


		//usleep(1000);
		close(sock);
	}
}



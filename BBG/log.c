
/*******************************************************************************************************
*
* UNIVERSITY OF COLORADO BOULDER
*
* @file log.c
* @brief Log data from all files to log file
* 
* @author Kiran Hegde and Gautham 
* @date  4/29/2018
* @tools vim editor
*
********************************************************************************************************/



#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include "log.h"
#include <errno.h>
#include <string.h>


void LOG(uint32_t loglevel, uint32_t log_source, char *msg, uint32_t value,uint32_t timestamp)
{
	//sem_wait(log_lock);
        pthread_mutex_lock(&log_lock);
        Logger_t logging;
	printf("5");
        logging.timestamp=timestamp;
        printf("6");
	logging.log_level = loglevel;
        logging.log_source = log_source;
	    logging.value = value;
        memcpy(logging.message, msg, MSG_SIZE);

        
	printf("7\n");
       if((mq_send(log_q, &logging, sizeof(Logger_t),0))==-1)
        {
                printf("cant send message to process1 and returned %d\n", errno);
        }
	 
      	
	printf("8");
       pthread_mutex_unlock(&log_lock);

	//sem_post(log_lock);
}

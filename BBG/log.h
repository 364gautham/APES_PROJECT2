#ifndef _LOG_H

#define _LOG_H

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <semaphore.h>
#include <mqueue.h>


mqd_t log_q;
extern pthread_mutex_t log_lock;

#define LOG_QUEUE "/logqueu1"
#define SOCKET_QUEUE "/socketqueu1"
#define SOCK_REPLY_QUEUE "/sockqueue2"
#define HB_QUEUE_COMM "/heartbeat1"
#define HB_QUEUE_SOCKET "/heartbeat2"
#define HB_QUEUE_LOG "/heartbeat3"

#define MSG_SIZE                (40)

#define LOG_LEVEL_INIT          (0x5)
#define LOG_LEVEL_INFO 		0X6
#define LOG_LEVEL_ERROR         (0x7)
#define LOG_LEVEL_HEARTBEAT     (0x11)

#define LOG_SOURCE_MAIN    (0xa)
#define LOG_SOURCE_COMM     (0xb)
#define LOG_SOURCE_LOGGER   (0xc)
#define LOG_SOURCE_SERVER   (0xe)
#define LOG_SOURCE_DECISION (0xf)

#define HB_COMM_VAL 0x01
#define HB_SOCK_VAL 0x02
#define HB_LOGGER_VAL 0x03




typedef struct log
{
	uint32_t timestamp;
	uint32_t log_level;
	uint32_t log_source;
	char message[MSG_SIZE];
	uint32_t value;
//	uint32_t checksum;
}Logger_t;

void LOG(uint32_t loglevel, uint32_t log_source, char *msg, uint32_t value,uint32_t timestamp);

#endif

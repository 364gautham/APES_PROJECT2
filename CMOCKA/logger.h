/*
 * logger.h
 *
 *  Created on: Apr 27, 2018
 *      Author: KiranHegde
 */

#ifndef INCLUDE_LOGGER_H_
#define INCLUDE_LOGGER_H_

#include <time.h>
#include <stdint.h>

#define MSG_SIZE                (30)
#define LOG_SOURCE_GESTURE      (0x1)
#define LOG_SOURCE_RELAY        (0x2)
#define LOG_SOURCE_MAIN         (0x3)
#define LOG_SOURCE_COMM         (0x4)
#define LOG_SOURCE_CLIENT       (0x12)

#define LOG_LEVEL_INIT          (0x5)
#define LOG_LEVEL_INFO          (0x6)
#define LOG_LEVEL_ERROR         (0x7)
#define LOG_LEVEL_WARNING       (0x8)
#define LOG_LEVEL_EXIT          (0x9)
#define LOG_LEVEL_HEARTBEAT     (0x11)
#define LOG_VALUE               (0x20)

typedef struct log
{
    uint32_t value;
    uint32_t timestamp;
    uint32_t log_level;
    uint32_t log_source;
    char msg[MSG_SIZE];
}Logger_t;

#endif /* INCLUDE_LOGGER_H_ */

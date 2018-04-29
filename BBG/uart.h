#ifndef UART_H
#define UART_H

#include "log.h"

/* file descriptor UART device*/
extern int file;

/* function to initialize uart communication*/

void uart_init();

/* function to read a byte from UART RX*/

void read_byte(int file,char *receive);

/* function to read a string */
void read_string(int file,char *string);

/* functin to send a byte through UART*/

void send_byte(int file,char *send);

void read_struct(int file, Logger_t* data);




#endif

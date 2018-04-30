#ifndef _uart_H_

#define _uart_H_

#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
		




int uart_init();
int read_byte();

typedef enum { SUCCESS, ERROR}msg_t;
msg_t mesg_queue();

#endif

		

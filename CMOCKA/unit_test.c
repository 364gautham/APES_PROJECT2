
#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
//#include "socket.h"
#include "uart.h"

int result;

void test_log()
{
	uint8_t result, source = 1, level = 2, value=5, ptr_value;
	char *ptr = &ptr_value;
	result = LOG(source, level, ptr, value);
	assert_int_equal(1, result);	
}

void test_startuptest()
{
	uint8_t result, source = 1, level = 2, value=5, ptr_value;
	char *ptr = &ptr_value;
	result = StartupTest();
	assert_int_equal(1, result);	
}

void test_uartconfig()
{
	
	int test;
	int fd,ans =1,

	result = uart_init();
	assert_int_equal(result,ans);
}

void test_uartread()
{
	int test ;
	int res=1;

	result = read_byte();
	assert_int_equal(result,res);
}

void test_messageque()
{
	result = mesg_queue();

	assert_int_equal(result,SUCCESS);
}

int main()
{

	const struct CMUnitTest tests[] =
	{	
		cmocka_unit_test(test_log),
		cmocka_unit_test(test_startuptest),
		cmocka_unit_test(test_uartconfig),
		cmocka_unit_test(test_uartread),
		cmocka_unit_test(test_messageque),
		
	};

	return cmocka_run_group_tests(tests, NULL, NULL);

}



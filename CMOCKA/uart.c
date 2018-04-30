#include <termios.h>
#include <mqueue.h>
#include "uart.h"

int file;


int uart_init()
{
      
      /* UART device driver file open for UART protocol*/

   // file = open("/dev/ttyO4", O_RDWR | O_NOCTTY | O_SYNC)
       
    /* structure configuration */
   struct termios option;

   tcgetattr(file,&option);
   option.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON | IGNPAR);
   option.c_oflag = 0;
   option.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
   option.c_cc[VMIN] = 1;
   option.c_cc[VTIME] = 0;

  if(cfsetispeed(&option, B115200) || cfsetospeed(&option, B115200))
 	  perror("ERROR in baud set\n");

  if(tcsetattr(file, TCSAFLUSH,& option) < 0)
	 perror("ERROR in set attr\n");

  return 1;

}



int read_byte()
{
  int count=1;
  char *rec;
  
  //count = read (file,rec,1))<0)
  if(count == 1 )
    return 1;

}

msg_t mesg_queue()
{
  mqd_t que;
  struct mq_attr attr_log;
    que = mq_open("/logq", O_RDWR, 0666, &attr_log);
    //if(que != -1)
      return SUCCESS;

    return ERROR;
}


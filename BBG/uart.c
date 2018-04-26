#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include "uart.h"


/* file descriptor for UART*/
int file;

void uart_init()
{
      
      /* UART device driver file open for UART protocol*/

      if((file = open("/dev/ttyO4", O_RDWR | O_NOCTTY | O_NDELAY))<0){
        perror("UART: Failed to open the file.\n");
        return -1;
      }

    /* structure configuration */
   struct termios options;
   tcgetattr(file, &options);
   options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
   options.c_iflag = IGNPAR | ICRNL;
   tcflush(file, TCIFLUSH);
   tcsetattr(file, TCSANOW, &options);

}


void read_byte(int file,char *receive)
{
  int count=0;
   if( (count = read (file,receive,1))<0)
  {
      perror("read:");
      return -1;
  } 

  printf("received byte %c\n".*receive);

}

void read_string(int file,char *string)
{
  int i=0;
  do{
        read_byte(file,&string[i])
        //i++;
    }while(string[i++]=='\0');
}
/*
void read_struct()
{

}

*/


void send_byte(int file,char *send)
{

  int count = 0;
  if ((count = write(file, send, strlen(send)+1))<0){
      perror("Failed to send:\n");
      return -1;
   }

}

/*
int main(){
   int file, count;
   char *str1 = "gautham";
   // send the string plus the null character

   if ((count = write(file, str1, strlen(str1)+1))<0){
      perror("Failed to write to the output\n");
      return -1;
   }


  usleep(100000);
	int count1 =0;
  char recv[100];
 if( (count1 = read (file,recv,100))<0)
	{
	perror("read:");
	return -1;
	}	
	

   close(file);
   return 0;
}

*/



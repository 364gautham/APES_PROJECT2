#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <sys/types.h>
#include "uart.h"
#include <sys/stat.h>
#include <fcntl.h>
#include "log.h"


void uart_init()
{
      
      /* UART device driver file open for UART protocol*/

    if((file = open("/dev/ttyO4", O_RDWR | O_NOCTTY | O_SYNC))<0){
        perror("UART: Failed to open the file.\n");
        return -1;
    }
    /* structure configuration */
   struct termios option;

   tcgetattr(file,&option);
   option.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON | IGNPAR);
   option.c_oflag = 0;
   option.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
   option.c_cc[VMIN] = 1;
   option.c_cc[VTIME] = 0;

if(cfsetispeed(&option, B57600) || cfsetospeed(&option, B57600))
 	perror("ERROR in baud set\n");

if(tcsetattr(file, TCSAFLUSH,& option) < 0)
	perror("ERROR in set attr\n");

}



void read_byte(int file,char *receive)
{
  int count=0;
   if( (count = read (file,receive,1))<0)
  {
      perror("read:");
      return -1;
  } 

  //printf("Val : %d\n",count);

}

void read_string(int file,char *string)
{
  int i=0;
  do{
        read_byte(file,&string[i]);
        //i++;
    }while(string[i++]!='\0');
}

void read_struct(int file,Logger_t* data)
{
	int count =0;
	printf("hi33");
	char recv[sizeof(Logger_t)];
	
	read_byte(file,&(data->log_level));
    	//read_string(descriptor, (char*)&(recvd_struct->eeprom_data));
       // printf("Sensor data %d\n",(recvd_struct->sensor_data));

printf("hi");

  //:printf("time %ld\n", data->timestamp); 
  printf("log level %d",data->log_level);
   /*printf("log level %d",data->log_source);
  printf("log level %d",data->message);
  printf("log level %d",data->value);
*/


}



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



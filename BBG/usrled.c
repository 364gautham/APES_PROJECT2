/*******************************************************************************************************
*
* UNIVERSITY OF COLORADO BOULDER
*
* @file userled.c
* User led used to indicate errors through led 
* @author Kiran Hegde and Gautham 
* @date  4/29/2018
* @tools vim editor
*
********************************************************************************************************/


#include "usrled.h"

int identification_led()
{

	 FILE * led =NULL;
	  const char* LEDBrightness = "/sys/class/leds/beaglebone:green:usr0/brightness";
	    if((led=fopen(LEDBrightness,"r+"))!=NULL)
		      {
			         fwrite("1",sizeof(char),1,led);
				    fclose(led);

				      }

	      sleep(2);

	        if((led=fopen(LEDBrightness,"r+"))!=NULL)
			  {
				     fwrite("0",sizeof(char),1,led);
				        fclose(led);

					  }
		 return 0;
}

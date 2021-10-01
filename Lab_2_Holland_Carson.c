/*
 * Lab_Holland_Carson.c
 * Date: 28 September 2021
 * Written by: Carson Holland
 * Communications written by Dr. Stapleton
 * The purpose of this program is to create a
 * traffic light system using LED's
 */

#include "ee3420_pi.h"

//variables for this program
char host_receive_buffer[81];
int host_receive_buffer_pointer=0;
char host_received_char;
int host_received_instruction_complete=0;
char host_buffer[81];

char test_string[]="ONE:TWO:Three:Four:five:sIx:?\n";

char command_string[100];
char command_parameters[max_parameter_limit][max_parameter_length];  

#define RED 5 
#define YELLOW 6
#define GREEN 23
#define BLUE 20
#define PUSHBUTTON 17  

int main()
{
	//START UP
	if(ee3420_startup() <0)
	{ return(ee3420_shutdown()); }
	
	//Variable Declarations
	int Traffic = 1;
	int loop_active = 1;
	char keys_received[3]={0,0,0};
	gpioSetMode(RED, PI_OUTPUT);
	gpioSetMode(YELLOW, PI_OUTPUT);
	gpioSetMode(GREEN, PI_OUTPUT);
	gpioSetMode(BLUE, PI_OUTPUT);
	gpioSetMode(PUSHBUTTON, PI_INPUT);
	
	while(loop_active)
	{ //WHILE LOOP TOP BRACKET
	
	//GET KEYPAD INPUT-----------------------------------
	strcpy(command_string,"KEYPAD:?\n");
		if(1)
		{
			printf("Sending request ... %s",command_string);
		}
		//this is the actual request send
		serial_send_request(serial_handle, command_string, serial_receive_buffer);
		if(1)
		{
			printf("Response ...\n");
			printf("%s\n",serial_receive_buffer);
		}
		//check for errors from Mega and parameterize
		if(process_serial_response(serial_receive_buffer) < 0)
		{
			display_serial_device_error(serial_device.last_error_code);
		}
		
		//read key returned or "NONE" message
		if(!strcmp(&serial_command_parameters[1][0],"NONE"))
		{
			//if response is "NONE", then no key entered
			if(1)
			{
				printf("N\n");
			}
		}
		else
		{
			//If response is not "NONE" assume key received
			//move last two keystrokes back in buffer
			keys_received[2]=keys_received[1]; 
			keys_received[1]=keys_received[0];
			keys_received[0]=serial_command_parameters[1][0]; //current key
			if(1)
			{
				printf("%c",keys_received[0]);
			}			
		}
			gpioDelay(500000); 
			
		
	//IF * IS PRESSED 3 TIMES EXIT--------------------------------
	if(keys_received[0] == '*')
	{
		printf("* WAS PRESSED\n");
		loop_active = 0;
	}
	
	//CLEAR LCD SCREEN------------------------------------
	sprintf(command_string,"LCD:CLEAR\n");
				if(1)
				{
					printf("Sending request ... %s",command_string);
				}
				//this is the actual request send
				serial_send_request(serial_handle, command_string, serial_receive_buffer);
				if(1)
				{
					printf("Response ...\n");
					printf("%s\n",serial_receive_buffer);
				}
				//check for errors from Mega and parameterize
				if(process_serial_response(serial_receive_buffer) < 0)
				{
					display_serial_device_error(serial_device.last_error_code);
				}
	
	switch(Traffic)
		{//Switch Top Bracket
			case 1:
			printf("Case 1\n");
			
			sprintf(command_string,"LCD:TEXT:Case 1\n");
			if(1)
				{
					printf("Sending request ... %s",command_string);
				}
				//this is the actual request send
				serial_send_request(serial_handle, command_string, serial_receive_buffer);
			if(1)
				{
					printf("Response ...\n");
					printf("%s\n",serial_receive_buffer);
				}
				//check for errors from Mega and parameterize
			if(process_serial_response(serial_receive_buffer) < 0)
				{
					display_serial_device_error(serial_device.last_error_code);
				}
			
			//N-S Road
			gpioWrite(RED, 1);//ON
			gpioWrite(YELLOW, 0);
			gpioWrite(GREEN, 0);
			gpioWrite(BLUE, 0);
					
			//E-W Road
			strcpy(command_string,"LED:RED:1\n");//ON
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:YELLOW:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:GREEN:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:BLUE:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			//STAY HERE FOR 3 SECONDS
			gpioDelay(3000000);
			
			if(gpioRead(PUSHBUTTON))
				{
					Traffic = 9;
				}
			
			else
				{								
					Traffic = 2;
				}
				
			break;
					
			case 2:
			printf("Case 2\n");
			
			sprintf(command_string,"LCD:TEXT:Case 2\n");
			if(1)
				{
					printf("Sending request ... %s",command_string);
				}
				//this is the actual request send
				serial_send_request(serial_handle, command_string, serial_receive_buffer);
			if(1)
				{
					printf("Response ...\n");
					printf("%s\n",serial_receive_buffer);
				}
				//check for errors from Mega and parameterize
			if(process_serial_response(serial_receive_buffer) < 0)
				{
					display_serial_device_error(serial_device.last_error_code);
				}

			//N-S Road
			gpioWrite(RED, 0);
			gpioWrite(YELLOW, 0);
			gpioWrite(GREEN, 1);//ON
			gpioWrite(BLUE, 0);
					
			//E-W Road
			strcpy(command_string,"LED:RED:1\n");//ON
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:YELLOW:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:GREEN:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:BLUE:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			//STAY HERE FOR 7 SECONDS
			gpioDelay(7000000);
			
			if(gpioRead(PUSHBUTTON))
				{
					Traffic = 7;
				}
			
			else
				{
					Traffic = 3;
				}
			
			break;
			
			case 3:
			printf("Case 3\n");
			
			sprintf(command_string,"LCD:TEXT:Case 3\n");
			if(1)
				{
					printf("Sending request ... %s",command_string);
				}
				//this is the actual request send
				serial_send_request(serial_handle, command_string, serial_receive_buffer);
			if(1)
				{
					printf("Response ...\n");
					printf("%s\n",serial_receive_buffer);
				}
				//check for errors from Mega and parameterize
			if(process_serial_response(serial_receive_buffer) < 0)
				{
					display_serial_device_error(serial_device.last_error_code);
				}

			//N-S Road
			gpioWrite(RED, 0);
			gpioWrite(YELLOW, 1);//ON
			gpioWrite(GREEN, 0);
			gpioWrite(BLUE, 0);
					
			//E-W Road
			strcpy(command_string,"LED:RED:1\n");//ON
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:YELLOW:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:GREEN:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:BLUE:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			//STAY HERE FOR 2 SECONDS
			gpioDelay(2000000);
			
			if(gpioRead(PUSHBUTTON))
				{
					Traffic = 9;
				}
			else
				{
					Traffic = 4;
				}
			break;
			
			case 4:
			printf("Case 4\n");
			
			sprintf(command_string,"LCD:TEXT:Case 4\n");
			if(1)
				{
					printf("Sending request ... %s",command_string);
				}
				//this is the actual request send
				serial_send_request(serial_handle, command_string, serial_receive_buffer);
			if(1)
				{
					printf("Response ...\n");
					printf("%s\n",serial_receive_buffer);
				}
				//check for errors from Mega and parameterize
			if(process_serial_response(serial_receive_buffer) < 0)
				{
					display_serial_device_error(serial_device.last_error_code);
				}

			//N-S Road
			gpioWrite(RED, 1);//ON
			gpioWrite(YELLOW, 0);
			gpioWrite(GREEN, 0);
			gpioWrite(BLUE, 0);
					
			//E-W Road
			strcpy(command_string,"LED:RED:1\n");//ON
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:YELLOW:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:GREEN:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:BLUE:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			//STAY HERE FOR 3 SECONDS
			gpioDelay(3000000);
			
			if(gpioRead(PUSHBUTTON))
				{
					Traffic = 7;
				}
			
			else
				{
					Traffic = 5;
				}
			break;
			
			case 5:
			printf("Case 5\n");
			
			sprintf(command_string,"LCD:TEXT:Case 5\n");
			if(1)
				{
					printf("Sending request ... %s",command_string);
				}
				//this is the actual request send
				serial_send_request(serial_handle, command_string, serial_receive_buffer);
			if(1)
				{
					printf("Response ...\n");
					printf("%s\n",serial_receive_buffer);
				}
				//check for errors from Mega and parameterize
			if(process_serial_response(serial_receive_buffer) < 0)
				{
					display_serial_device_error(serial_device.last_error_code);
				}

			//N-S Road
			gpioWrite(RED, 1);//ON
			gpioWrite(YELLOW, 0);
			gpioWrite(GREEN, 0);
			gpioWrite(BLUE, 0);
					
			//E-W Road
			strcpy(command_string,"LED:RED:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:YELLOW:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:GREEN:1\n");//ON
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:BLUE:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			//STAY HERE FOR 7 SECONDS
			gpioDelay(7000000);
			
			if(gpioRead(PUSHBUTTON))
				{
					Traffic = 8;
				}	
			else
				{
					Traffic = 6;
				}
			break;
			
			case 6:
			printf("Case 6\n");
			
			sprintf(command_string,"LCD:TEXT:Case 6\n");
			if(1)
				{
					printf("Sending request ... %s",command_string);
				}
				//this is the actual request send
				serial_send_request(serial_handle, command_string, serial_receive_buffer);
			if(1)
				{
					printf("Response ...\n");
					printf("%s\n",serial_receive_buffer);
				}
				//check for errors from Mega and parameterize
			if(process_serial_response(serial_receive_buffer) < 0)
				{
					display_serial_device_error(serial_device.last_error_code);
				}

			//N-S Road
			gpioWrite(RED, 1);//ON
			gpioWrite(YELLOW, 0);
			gpioWrite(GREEN, 0);
			gpioWrite(BLUE, 0);
					
			//E-W Road
			strcpy(command_string,"LED:RED:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:YELLOW:1\n");//ON
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:GREEN:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:BLUE:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			//STAY HERE FOR 2 SECONDS
			gpioDelay(2000000);
			
			if(gpioRead(PUSHBUTTON))
				{
					Traffic = 9;
				}
			else	
				{	
					Traffic = 1;
				}
			break;
			
			case 7:
			printf("Case 7\n");
			
			sprintf(command_string,"LCD:TEXT:Case 7\n");
			if(1)
				{
					printf("Sending request ... %s",command_string);
				}
				//this is the actual request send
				serial_send_request(serial_handle, command_string, serial_receive_buffer);
			if(1)
				{
					printf("Response ...\n");
					printf("%s\n",serial_receive_buffer);
				}
				//check for errors from Mega and parameterize
			if(process_serial_response(serial_receive_buffer) < 0)
				{
					display_serial_device_error(serial_device.last_error_code);
				}

			//N-S Road
			gpioWrite(RED, 0);
			gpioWrite(YELLOW, 1);//ON
			gpioWrite(GREEN, 0);
			gpioWrite(BLUE, 0);
					
			//E-W Road
			strcpy(command_string,"LED:RED:1\n");//ON
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:YELLOW:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:GREEN:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:BLUE:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			//STAY HERE FOR 2 SECONDS
			gpioDelay(2000000);
			
			Traffic = 9;
			break;
			
			case 8:
			printf("Case 8\n");
			
			sprintf(command_string,"LCD:TEXT:Case 8\n");
			if(1)
				{
					printf("Sending request ... %s",command_string);
				}
				//this is the actual request send
				serial_send_request(serial_handle, command_string, serial_receive_buffer);
			if(1)
				{
					printf("Response ...\n");
					printf("%s\n",serial_receive_buffer);
				}
				//check for errors from Mega and parameterize
			if(process_serial_response(serial_receive_buffer) < 0)
				{
					display_serial_device_error(serial_device.last_error_code);
				}

			//N-S Road
			gpioWrite(RED, 1);//ON
			gpioWrite(YELLOW, 0);
			gpioWrite(GREEN, 0);
			gpioWrite(BLUE, 0);
					
			//E-W Road
			strcpy(command_string,"LED:RED:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:YELLOW:1\n");//ON
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:GREEN:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:BLUE:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			//WAIT HERE FOR 2 SECONDS
			gpioDelay(2000000);
			Traffic = 9;
			break;
			
			case 9:
			printf("Case 9\n");
			
			sprintf(command_string,"LCD:TEXT:Case 9\n");
			if(1)
				{
					printf("Sending request ... %s",command_string);
				}
				//this is the actual request send
				serial_send_request(serial_handle, command_string, serial_receive_buffer);
			if(1)
				{
					printf("Response ...\n");
					printf("%s\n",serial_receive_buffer);
				}
				//check for errors from Mega and parameterize
			if(process_serial_response(serial_receive_buffer) < 0)
				{
					display_serial_device_error(serial_device.last_error_code);
				}

			//N-S Road
			gpioWrite(RED, 1);//ON
			gpioWrite(YELLOW, 0);
			gpioWrite(GREEN, 0);
			gpioWrite(BLUE, 1);//ON
					
			//E-W Road
			strcpy(command_string,"LED:RED:1\n");//ON
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:YELLOW:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:GREEN:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:BLUE:1\n");//ON
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			gpioDelay(20000);
			
			strcpy(command_string,"LED:BUZZER:1\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			
			//WAIT HERE FOR 10 SECONDS
			gpioDelay(10000000);
			
			strcpy(command_string,"LED:BUZZER:0\n");
			printf("Sending request ... %s",command_string);
			serial_send_request(serial_handle, command_string, serial_receive_buffer);
			process_serial_response(serial_receive_buffer);
			
			Traffic = 1;
			break;
		}//SWITCH STATEMENT BRACKET
	}//WHILE LOOP BOTTOM BRACKET
	
	//TEST CODE!!!! ***********************************************************
	/*
	//Turn on PI LED's---------
	gpioWrite(RED, 1);
	gpioWrite(YELLOW, 1);
	gpioWrite(GREEN, 1);
	gpioWrite(BLUE, 1);
						
	//Turn on all Arduino LED's but one	--------		
	strcpy(command_string,"LED:RED:1\n");
	printf("Sending request ... %s",command_string);
	serial_send_request(serial_handle, command_string, serial_receive_buffer);
	process_serial_response(serial_receive_buffer);
	gpioDelay(500000);
	
	 
	strcpy(command_string,"LED:BLUE:1\n");
	printf("Sending request ... %s",command_string);
	serial_send_request(serial_handle, command_string, serial_receive_buffer);
	process_serial_response(serial_receive_buffer);
	gpioDelay(500000);
	
 
	strcpy(command_string,"LED:YELLOW:1\n");
	printf("Sending request ... %s",command_string);
	serial_send_request(serial_handle, command_string, serial_receive_buffer);
	process_serial_response(serial_receive_buffer);
	gpioDelay(500000);
	
	//loop to blink the LED---------
	for(int i=0; i<5; i++)
	{
	strcpy(command_string,"LED:GREEN:0\n");
	printf("Sending request ... %s",command_string);
	serial_send_request(serial_handle, command_string, serial_receive_buffer);
	process_serial_response(serial_receive_buffer);
	gpioDelay(500000);  
	strcpy(command_string,"LED:GREEN:1\n");
	printf("Sending request ... %s",command_string);
	serial_send_request(serial_handle, command_string, serial_receive_buffer);
	process_serial_response(serial_receive_buffer);
	gpioDelay(500000);   
	}
	
	*/
	//TEST CODE********************************************************************
	
	//Turn off Arduino LED's--------------
	strcpy(command_string,"LED:RED:0\n");
	printf("Sending request ... %s",command_string);
	serial_send_request(serial_handle, command_string, serial_receive_buffer);
	process_serial_response(serial_receive_buffer);
	gpioDelay(500000); 
		
	strcpy(command_string,"LED:BLUE:0\n");
	printf("Sending request ... %s",command_string);
	serial_send_request(serial_handle, command_string, serial_receive_buffer);
	process_serial_response(serial_receive_buffer);
	gpioDelay(500000); 
		
	strcpy(command_string,"LED:YELLOW:0\n");
	printf("Sending request ... %s",command_string);
	serial_send_request(serial_handle, command_string, serial_receive_buffer);
	process_serial_response(serial_receive_buffer);
	gpioDelay(500000); 
	
	strcpy(command_string,"LED:GREEN:0\n");
	printf("Sending request ... %s",command_string);
	serial_send_request(serial_handle, command_string, serial_receive_buffer);
	process_serial_response(serial_receive_buffer);
	gpioDelay(500000); 
	
	//Turn off PI LED's---------------------------
	gpioWrite(RED, 0);
	gpioWrite(YELLOW, 0);
	gpioWrite(GREEN, 0);
	gpioWrite(BLUE, 0);
	gpioDelay(1000000); 

	//LCD PRINT PROGRAM IS STOPPING
	
	sprintf(command_string,"LCD:CLEAR\n");
			if(1)
				{
					printf("Sending request ... %s",command_string);
				}
				//this is the actual request send
				serial_send_request(serial_handle, command_string, serial_receive_buffer);
			if(1)
				{
					printf("Response ...\n");
					printf("%s\n",serial_receive_buffer);
				}
				//check for errors from Mega and parameterize
			if(process_serial_response(serial_receive_buffer) < 0)
				{
					display_serial_device_error(serial_device.last_error_code);
				}
	sprintf(command_string,"LCD:TEXT:SHUTDOWN\n");
			if(1)
				{
					printf("Sending request ... %s",command_string);
				}
				//this is the actual request send
				serial_send_request(serial_handle, command_string, serial_receive_buffer);
			if(1)
				{
					printf("Response ...\n");
					printf("%s\n",serial_receive_buffer);
				}
				//check for errors from Mega and parameterize
			if(process_serial_response(serial_receive_buffer) < 0)
				{
					display_serial_device_error(serial_device.last_error_code);
				}

	
	return(ee3420_shutdown());	
}//INT MAIN BRACKET

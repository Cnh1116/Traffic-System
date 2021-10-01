#ifndef EE3420_PI_H
#define EE3420_PI_H

/*
 * ee3420_pi.h
 * revision date: 15 August 2021
 * Written by: William Stapleton
 * The purpose of this header file is to establish common variables and functions
 * needed for the EE 3420 laboratory assignments.
 * This code is designed for use on the Raspberry Pi 4 using the "pigpio" library.
 * The communication protocol is required for operating cooperatively with an
 * Arduino Mega 2560 in the role of "smart peripheral".
 * The Arduino Mega will be operating with a complementary header file filling this role.
 * When compiling using "gcc" or "g++" you must include the following command line parameters
 * in order to support the "pigpio" library ...
 * -lpigpio -lpthread -lrt
 */
 
//Standard C system libraries
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <math.h>

//Other libraries supplied by William Stapleton
#include "ASCII.h"
#include "VT100.h"

//The "pigpio" library for GPIO control on the Raspberry Pi
#include <pigpio.h>

//A handy way to specify the Nth bit 
//gives the value 2^N
#ifndef BIT
#define BIT(N) (1 << (N))
#endif

//Error codes for the communications protocol
#define ERROR_UNKNOWN_COMMAND -1
#define ERROR_INVALID_PARAMETER -2
#define ERROR_INVALID_DATA -3
#define ERROR_UNAVAILABLE -4
#define ERROR_TIMEOUT -5

//Strings describing the communications protocol error codes
char *error_messages[6] = {"","UNKNOWN COMMAND", "INVALID PARAMETER", "INVALID_DATA", "UNAVAILABLE", "TIMEOUT"};

//this data structure will hold a local copy of the status of systems on the Mega 2560
struct mega2560_data 
{
int led_builtin;
int led_red;
int led_green;
int led_blue;
int led_yellow;
int led_white;
int keypad_value;
int last_error_code;	
int adc_maxval;
int adc_values[16];
	
} serial_device;

//This function will initialize the data structure describing the Mega 2560
void mega2560_device_initialize(struct mega2560_data device)
{
	device.led_builtin=0;
	device.led_red=0;
	device.led_green=0;
	device.led_blue=0;
	device.led_yellow=0; 
	device.led_white=0;
	device.keypad_value=0;
	device.last_error_code=0;
	device.adc_maxval=1023;
	for(int i=0; i<16; i++) {device.adc_values[i]=0;}
}

//When a command is received, it will be split into parameters
//max_parameter_limit determines the maximum number of parameters to extract
#define max_parameter_limit 20
//max_parameter_length determines the maximum length of a single parameter string
#define max_parameter_length 40
//This array holds the parameters after splitting the command
char serial_command_parameters[max_parameter_limit][max_parameter_length];
//a variable used to store the number of parameters
int command_parameter_counter;


//a collection of characters which can be used to separate parameters on the command line
//note that ":" is the default used in the documentation but "," will work.
char parameter_separators[]=" :,\r\n";

//the complete command as a string
char serial_command_string[81];
//a variable to hold an error code when decoding an instruction
int serial_error_code;

//The Pi will commnicate with the Mega over a virtual UART over USB
//This is most commonly named "/dev/ttyACM0" but may be "/dev/ttyACM1", "/dev/ttyACM2", etc.
char serial_port_name[40]="/dev/ttyACM0";
//The "pigpio" serOpen() function returns an integer handle for the serial port
int serial_handle;

//string to receive a command over serial and related variables
char serial_receive_buffer[81];
int serial_receive_buffer_pointer=0;
char serial_received_char;
int serial_received_instruction_complete=0;
char serial_buffer[81];

//Variables to receive the device type and name of the peripheral
char serial_device_type[40] = "UNKNOWN";  //expected "MEGA2560"
char serial_device_name[40] = "UNKNOWN";  //can be anything

//The Raspberry Pi 4 should identify as type "PI4"
char my_device_type[]="PI4";
char my_device_name[]="BerryGood";  


//variables for handling time data
// the time_t type is a single (long) integer normally counting the number of seconds since the "epoch"
//The "epoch" is 12:00:00 a.m. on 1 January, 1970 AD
time_t time_as_integer;
time_t start_time;
time_t end_time;
time_t now;

// the struct tm type separates the time into parts useful to humans
// the contents of struct tm are shown in the comments below and defined in <time.h>
struct tm *time_as_structure;

//DO NOT UNCOMMENT THIS BLOCK
//
// * struct tm {
//   int tm_sec;         /* seconds,  range 0 to 59          */
//   int tm_min;         /* minutes, range 0 to 59           */
//   int tm_hour;        /* hours, range 0 to 23             */
//   int tm_mday;        /* day of the month, range 1 to 31  */
//   int tm_mon;         /* month, range 0 to 11             */
//   int tm_year;        /* The number of years since 1900   */
//   int tm_wday;        /* day of the week, range 0 to 6    */
//   int tm_yday;        /* day in the year, range 0 to 365  */
//   int tm_isdst;       /* daylight saving time             */
//};
//
//END OF DO NOT UNCOMMENT BLOCK

int hours_12;
char ampm;

//the native pigpio serReadByte() instruction is blocking
//I have included a non-blocking alternative serGetc()
//The serGetc() can accept a non-zero timeout to be semi-blocking
int serial_receive_timeout_length=0;  //0 means no timeout, positive means milliseconds (10-60000)
//a flag variable used with the timout
int serial_receive_timeout_flag;
//the timeout is implemented using a pigpio timer function
int serial_receive_timeout_timer_channel=0;

//callback functions for the pigpio timer used with serGetc() timeout
void set_serial_receive_timeout_flag()
{ serial_receive_timeout_flag=1; }
void clear_serial_receive_timeout_flag()
{ serial_receive_timeout_flag=0; }

//function to safely set the timeout length for serGetc()
//the pigpio timer used can only accept times in the range 10-60000 milliseconds
int set_serial_receive_timeout_length(int millis)
{
	if(millis <= 0) {serial_receive_timeout_length=0;}  //no blocking
	else if (millis < 10) {serial_receive_timeout_length=10;}  //minimum time is 10 ms
	else if (millis <= 60000) {serial_receive_timeout_length=millis;}
	else {serial_receive_timeout_length=60000;}  //maximum time is 60000 ms = 60 s
	
	return(serial_receive_timeout_length);  //return actual timeout time after adjustment
}

//serPutc() is a rename of serWriteByte()
int serPutc(int handle, char c)
{
	return(serWriteByte(handle,c));
}

//serGetc() is a non-blocking or semi-blocking alternative to serReadByte()
int serGetc(int handle)
{
	int tempvar;
	
	clear_serial_receive_timeout_flag();  //reset timeout state
	if (serial_receive_timeout_length >0)  //if using timeout
	{
		if (serial_receive_timeout_length < 10) {serial_receive_timeout_length=10;}  //ensure minimum timeout length observed
		if (serial_receive_timeout_length > 60000) {serial_receive_timeout_length=60000;}  //ensure maximum timeout length observed
		//use pigpio timer to set flag after timeout time
		gpioSetTimerFunc(serial_receive_timeout_timer_channel, serial_receive_timeout_length, set_serial_receive_timeout_flag);
	}
	
	//wait while serial data is not available and timeout flag is not set
	while(((tempvar=serDataAvailable(handle))==0)&&(!serial_receive_timeout_flag)){}
	
	//this point is reached once a characte is read or tihe timeout flag is set
	//in either case turn off the timer
	gpioSetTimerFunc(serial_receive_timeout_timer_channel,serial_receive_timeout_length,NULL);
	
	//if error was received, return that error code forwarded from serDataAvailable()
	if(tempvar < 0) {return(tempvar);}
	
	//this is the case when timeout was invoked with no character received
	if ((serial_receive_timeout_length >0)&&(serial_receive_timeout_flag)) { return(PI_SER_READ_NO_DATA); }
	
	//if you make it here, a character is available on the serial port
	//read it and return the character
	return(serReadByte(handle)); 
}	

//serPuts() uses serPutc() to output a character string up to the null-terminator
int serPuts(int handle, char str[])
{
	int i, j, l, d;
	l = strlen(str);  //length of string to send
	d=0;
	if(!strcmp(serial_device_type,"")) {d=100;}	
	if(!strcmp(serial_device_type,"UNKNOWN")) {d=100;}
	if(!strcmp(serial_device_type,"MEGA2560") && (l > 7)) {d=100;}
	if(!strcmp(serial_device_type,"UNO") && (l > 3)) {d=100;}
	if(!strcmp(serial_device_type,"KL46Z") && (l > 3)) {d=10;}
	if(!strcmp(serial_device_type,"PI0")) {d=0;}
	if(!strcmp(serial_device_type,"PICO")) {d=0;}
	
	for(i=0; str[i]; i++)
	{
		if((j=serPutc(handle,str[i]))<0)
		{break;}
		
		//adding an inter-character delay for long strings on MEGA 2560
		//or unknown peripherals to prevent serial buffer overflow
		if( d > 0 ) { gpioDelay(d); }
	}
	if(j<0) {return(j);} else {return(strlen(str));}
}

//serGets() uses serGetc() to input a character string up to max_count characters.
//input of a newline ('\n') terminates the string at less than max_count
//the newline is not stored, rather a null ('\0') is stored in place of it
int serGets(int handle, char str[], int max_count)
{
	int i;
	int complete=0;
	for(i=0; ((i<max_count-1) && (!complete)); i++)
	{
		str[i]=serGetc(handle);  //read character using semi-blocking
		if(str[i]<0)  //if error or timeout, terminate string early
		{
			str[i]='\0';
			return(str[i]);
		}
		if(str[i]=='\n')
		{
			str[i]='\0';
			complete=1;
		}
	}
	str[max_count-1]='\0';  //null-terminate if maximum length
	return(strlen(str));
}

//This is the ENQ/ACK synchronization from the initiator (PI) side
int enquire_serial_device(int handle)
{
	int return_code=0;
	//printf("Sending ENQ\n");
	for(int i=1;i<21;i++)
	{
		printf("ENQ attempt %i of 20\n",i);
		serPutc(handle,ENQ); 
		int r1=serGetc(handle);
		if(r1 == ACK)
		{
			printf("ACK received\n");
			return_code=r1;
			break;
		}
		else if(r1 == NACK)
		{
			printf("NACK received\n");
			return_code=r1;
			break;
		}
		else if(r1 <0)
		{
			printf("Timeout\n");
			return_code=-1;
		}
		else
		{
			printf("Malformed response\n");
			return_code=-2;
		}				
	}
	return(return_code);	
}

//this function is used to split a command into parameters
int parse_command_string(char input_string[], char parameters[max_parameter_limit][max_parameter_length], char separators[])
{
	int command_counter;
	char * tok;
	
	for(command_counter=0; command_counter<max_parameter_limit; command_counter++)
	{
		parameters[command_counter][0]='\0';
	}
	
	command_counter=0;
	tok=strtok(input_string,separators);  //string tokenator function from string.h
	while((tok != NULL)&&(command_counter<max_parameter_limit))
	{
		//printf("TOKEN: %s\n",tok)
		strncpy ( &parameters[command_counter][0],tok, max_parameter_length );
		//printf("PARAMETER: %s\n",&command_parameters[command_counter][0]);
		command_counter++;
		tok=strtok(NULL,separators);		
	}
	command_parameter_counter=command_counter;
	return(command_counter);	
}
 
//change all lowercase characters in command string into uppercase
//this makes comparisons much easier
void make_command_uppercase(char cmd_str[])
{
	for(int i=0; i< (int)strlen(cmd_str); i++)
	{
		cmd_str[i]=toupper(cmd_str[i]);
	}
}

//change all lowercase characters in command parameters into uppercase
//this makes comparisons much easier
void make_command_parameters_uppercase(char parameters[max_parameter_limit][max_parameter_length])
{
	for(int i=0; i< max_parameter_limit; i++)
	{
		for(int j=0; j<(int)strlen(&parameters[i][0]); j++)
		{		
			parameters[i][j]=toupper(parameters[i][j]);
		}
	}
}

//function for the Pi to send a command string to Mega and wait for and receive response
int serial_send_request(int handle, char command[], char buffer[])
{
	buffer[0]='\0';
	//send the command string
	serPuts(handle, command);
	//wait for ACK or NACK
	while((serial_received_char=serGetc(handle)<0)) {}
	//store the following received string
	serGets(handle, buffer, 80);

	return(0);
}

//function for the Pi to send a command string to Mega without waiting for response string
int serial_send_command(int handle, char command[])
{
	//send string
	serPuts(handle, command);
	//wait for ACK or NACK
	while((serial_received_char=serGetc(handle)<0)) {}

	return(0);
}

//function to give text message based on communications protocol error	
void display_serial_device_error(int code)
{    
	printf("ERROR code %i is %s\n",code, error_messages[abs(code)]);
}
	
//This function processes the response strings from the Mega
//This function does a LOT of different things
//This is one of the functions you are  LIKELY to alter as the labs progress	
int process_serial_response(char serial_receive_buffer[])
{
    int return_code=0;  //initially assume all will work
    int led_temp;
    int serial_command_process_complete=0;	//initially haven't processed command
    //split the received string into parameters
    parse_command_string(serial_receive_buffer, serial_command_parameters, parameter_separators);
    make_command_parameters_uppercase(serial_command_parameters);
    
    //check for OK response
    //really common so you might want it to be checked first
    if( !strcmp(&serial_command_parameters[0][0],"OK") &&
	!serial_command_process_complete
      )
    {
	serial_command_process_complete=1;
    }
    //end of check for OK
    
    //begin ID:TYPE
    if( !strcmp(&serial_command_parameters[0][0],"ID") &&
        !strcmp(&serial_command_parameters[1][0],"TYPE") &&
	!serial_command_process_complete
      )
    {
	//store the type string in the serial_device_type global variable
        strcpy(serial_device_type,&serial_command_parameters[2][0]);
	serial_command_process_complete=1;
    }
    //end ID:TYPE
    
    //begin ID:NAME
    if( !strcmp(&serial_command_parameters[0][0],"ID") &&
        !strcmp(&serial_command_parameters[1][0],"NAME") &&
	!serial_command_process_complete
      )
    {
        //store the name string in the serial_device_name global variable
        strcpy(serial_device_name,&serial_command_parameters[2][0]);
	serial_command_process_complete=1;
    }
    //end ID:NAME
    
    //check for ID with bad parameter
    //must appear after all other ID cases
    if( !strcmp(&serial_command_parameters[0][0],"ID") &&
	!serial_command_process_complete
      )
    {
        return_code=-1;
	serial_command_process_complete=1;
    }
    //end of check for bad ID parameter   
    
    //begin LED:BUILTIN    
    if( !strcmp(&serial_command_parameters[0][0],"LED") &&
        !strcmp(&serial_command_parameters[1][0],"BUILTIN") &&
	!serial_command_process_complete
      )
    {
	led_temp=atoi(&serial_command_parameters[2][0]);
	if((led_temp>=0)&&(led_temp<=1))
	{ serial_device.led_builtin=led_temp; }
	else
	{ return_code=ERROR_INVALID_DATA; }
	serial_command_process_complete=1;
    }
    //end LED:BUILTIN
    
    //begin LED:RED
    if( !strcmp(&serial_command_parameters[0][0],"LED") &&
        !strcmp(&serial_command_parameters[1][0],"RED") &&
	!serial_command_process_complete
      )
    {
	led_temp=atoi(&serial_command_parameters[2][0]);
	if((led_temp>=0)&&(led_temp<=1))
	{ serial_device.led_red=led_temp; }
	else
	{ return_code=ERROR_INVALID_DATA; }
	serial_command_process_complete=1;
    }
    //end LED:RED
    
    //begin LED:GREEN
    if( !strcmp(&serial_command_parameters[0][0],"LED") &&
        !strcmp(&serial_command_parameters[1][0],"GREEN") &&
	!serial_command_process_complete
      )
    {
	led_temp=atoi(&serial_command_parameters[2][0]);
	if((led_temp>=0)&&(led_temp<=1))
	{ serial_device.led_green=led_temp; }
	else
	{ return_code=ERROR_INVALID_DATA; }   
	serial_command_process_complete=1; 
    }
    //end LED:GREEN
    
    //begin LED:BLUE   
    if( !strcmp(&serial_command_parameters[0][0],"LED") &&
        !strcmp(&serial_command_parameters[1][0],"BLUE") &&
	!serial_command_process_complete
      )
    {
	led_temp=atoi(&serial_command_parameters[2][0]);
	if((led_temp>=0)&&(led_temp<=1))
	{ serial_device.led_blue=led_temp; }
	else
	{ return_code=ERROR_INVALID_DATA; } 
	serial_command_process_complete=1;
    }
    //end LED:BLUE
    
    //begin LED:YELLOW
    if( !strcmp(&serial_command_parameters[0][0],"LED") &&
        !strcmp(&serial_command_parameters[1][0],"YELLOW") &&
	!serial_command_process_complete
      )
    {
	led_temp=atoi(&serial_command_parameters[2][0]);
	if((led_temp>=0)&&(led_temp<=1))
	{ serial_device.led_yellow=led_temp; }
	else
	{ return_code=ERROR_INVALID_DATA; }
	serial_command_process_complete=1;
    }
    //end LED:YELLOW
    
    //begin LED:WHITE    
    if( !strcmp(&serial_command_parameters[0][0],"LED") &&
        !strcmp(&serial_command_parameters[1][0],"WHITE") &&
	!serial_command_process_complete
      )
    {
	led_temp=atoi(&serial_command_parameters[2][0]);
	if((led_temp>=0)&&(led_temp<=1))
	{ serial_device.led_white=led_temp; }
	else
	{ return_code=ERROR_INVALID_DATA; }
	serial_command_process_complete=1;
    }
    //end LED:WHITE
    
    //check for LED with bad parameter
    //must appear after all other LED cases
     if( !strcmp(&serial_command_parameters[0][0],"LED") &&
	!serial_command_process_complete
      )
    {
        //store the name string in the serial_device_name global variable
        return_code=-2;
	serial_command_process_complete=1;
    }
    //end of check for bad LED parameter
    
    //begin ADC:<channel>:<value>
    if( !strcmp(&serial_command_parameters[0][0],"ADC") &&
      !serial_command_process_complete
      )
    {
	int response_adc_channel=atoi(&serial_command_parameters[1][0]);
	int response_adc_value=atoi(&serial_command_parameters[2][0]);
	//on Mega ...
	//channel must be 0-15
	//value must be 0-1023
	if((response_adc_channel >=0) &&
	   (response_adc_channel <= 15) &&
	   (response_adc_value >=0) &&
	   (response_adc_value < 1024)
	   )
	{
	    serial_device.adc_values[response_adc_channel]=response_adc_value;
        }
	else
	{ 
	    return_code=ERROR_INVALID_DATA;
        }
	serial_command_process_complete=1;
    }
    //begin ADC:<channel>:<value>
    
    //handle the case where the Mega reports an ERROR    
    if( !strcmp(&serial_command_parameters[0][0],"ERROR") &&
      !serial_command_process_complete
      )
    {
	serial_device.last_error_code=serial_error_code=atoi(&serial_command_parameters[1][0]);
        //printf("ERROR code %i is %s\n",serial_error_code, error_messages[abs(serial_error_code)]);
        return_code=serial_error_code;
	serial_command_process_complete=1;
    }
    
    //handle the case where the Mega sends an unknown response
    //this must be the last case tested after all other commands have been checked
    if( !serial_command_process_complete )
    {
       serial_device.last_error_code=return_code=ERROR_UNKNOWN_COMMAND;
    }
    
    return(return_code);  //return value of 0 means everything fine, <0 means error
}	

//The ee3420_startup() function should be the first function called in main()
//it will initialize the pigpio library functions
//it will establish communications with the Arduino Mega
//This should always be paired with ee3420_shutdown() at the end of the program
int ee3420_startup()
{
	int tempval;
	
	//read system time as seconds since epoch on January 1, 1970
	time(&time_as_integer);
	start_time=time_as_integer;
	//format time as structure of human interest values
	time_as_structure = localtime(&time_as_integer);
	//convert 24-hour dormat into 12 hour plus am/pm format
	if(time_as_structure->tm_hour <12)
	{ampm='A';}
	else
	{ampm='P';}
	hours_12=(time_as_structure->tm_hour)%12;
	if(hours_12 == 0) {hours_12=12;}
	
	printf("\nEE 3420 Initialization started at %02i/%02i/%4i, %02i:%02i:%02i %c.M.\n\n",
		(time_as_structure->tm_mon+1),
		time_as_structure->tm_mday,
		(time_as_structure->tm_year+1900),
		hours_12,
		time_as_structure->tm_min,
		time_as_structure->tm_sec,
		ampm	
	);
	
	printf("Initializing data structure for holding peripheral state.\n");
	printf("This assumes an Arduino Mega 2560 as the peripheral device.\n");
        mega2560_device_initialize(serial_device);
	
	/* initializing pigpio functions */
	/* remember to terminate at end of program */
	printf("Initializing pigpio functions ... ");

	//gpioIniitialize should allow control of GPIO pins
	if((tempval = gpioInitialise()) < 0)  //negative code means error, end program
	{
		printf("failed.\n");
		printf("EE 3420 Initialization aborted.\n");
		return(tempval);  
	}
	else
	{
		printf("success.\n");
	}
	
	/* initializing UART port */
	/* remember to close at end of program */
	//Virtual UART may be any port beginning "/dev/ttyACM"
	//We will try up to the first 10
	printf("Opening UART port for peripheral ...\n");
	serial_handle=-1;
	int i;
	for(i=0; ((i<10) && (serial_handle < 0)); i++)
	{
		sprintf(serial_port_name, "/dev/ttyACM%1i",i);
		printf("Trying port \"%s\" ... ",serial_port_name);
		if((serial_handle=serOpen(serial_port_name,115200,0))<0)
		{
			printf("failed.\n");
		}
		else
		{
			printf("success.\n");
		}
	}
	
	if(serial_handle < 0) 
	{
		printf("EE 3420 Initialization aborted.\n");
		return(serial_handle);
	}
	
	//setting the serGetc() timeout
	printf("Setting serial input timeout length\n");
	set_serial_receive_timeout_length(2000);  //1000 = 1000 ms = 1 second
	printf("Serial timeout set to %i milliseconds\n",serial_receive_timeout_length);
	
	printf("Establishing handshake with serial device\n");
	if(enquire_serial_device(serial_handle) < 0)
	{
		printf("Handshake failed.\n");
		printf("EE 3420 Initialization aborted.\n");
		return(-1);
	}
	else
	{
		printf("Handshake successful.\n");
	}
	
	serial_device_type[0]='\0';  //initially don't know peripheral type
	
	printf("Requesting peripheral device type.\n");
	strcpy(serial_command_string,"ID:TYPE:?\n");
	//printf("Sending request ... %s",command_string);
	serial_send_request(serial_handle, serial_command_string, serial_receive_buffer);
	//printf("Response ...\n");
	//printf("%s\n",serial_receive_buffer);
	
	if(process_serial_response(serial_receive_buffer) < 0)
	{
		display_serial_device_error(serial_device.last_error_code);
	}
	
    printf("The code was designed for peripherals of type MEGA2560.\n");
    printf("The reported peripheral device type is \"%s\"\n",serial_device_type);
    if( !strcmp(serial_device_type,"MEGA2560") )
    {
		printf("The peripheral matches. Everything should work.\n");
    } 
    else
    {
		printf("The peripheral does not match. Examples may not work.\n");
    }  
    
    	serial_device_name[0]='\0';  //initially don't know peripheral name
	
	printf("Requesting peripheral device name.\n");
	strcpy(serial_command_string,"ID:NAME:?\n");
	//printf("Sending request ... %s",command_string);
	serial_send_request(serial_handle, serial_command_string, serial_receive_buffer);
	//printf("Response ...\n");
	//printf("%s\n",serial_receive_buffer);
	
	if(process_serial_response(serial_receive_buffer) < 0)
	{
		display_serial_device_error(serial_device.last_error_code);
	}
	
	parse_command_string(serial_receive_buffer, serial_command_parameters, parameter_separators);
    /*for ease of comparision, make parameters uniformly uppercase */
    make_command_parameters_uppercase(serial_command_parameters);
	
    printf("The peripheral device name is \"%s\"\n",serial_device_name);

	
	return(0);
}


//The ee3420_shutdown() function should be the last function called in main()
//it will release the pigpio library functions and return control of GPIO pins to the OS
//it will close communications with the Arduino Mega
//This should always be paired with ee3420_startup() at the beginning of the program
int ee3420_shutdown()
{
	int tempvar=0;
	printf("\nEE 3420 Shutdown started\n");
	
	if(serial_handle < 0)
	{
		printf("No valid UART handle to close.\n");
	}
	else
	{
		//matches serOpen()
		printf("Closing UART port for peripheral ... ");
		if(serClose(serial_handle) < 0)		
		{
			printf("failed.\n");
			tempvar=-1;
		}
		else
		{
			printf("success.\n");
		}
	}	

	printf("Terminating pigpio functions.\n");
	//get final tally iof length of time pigpio was active before termination
	int gpio_seconds, gpio_microseconds;
	gpioTime(0,&gpio_seconds, &gpio_microseconds);
	gpioTerminate();  //matches gpioInitialize() at beginning
	printf("pigpio reports active for %i.%6i seconds before termination\n", gpio_seconds, gpio_microseconds);
	
	//recount start time
	time_as_structure = localtime(&start_time);
	//convert 24-hour format into 12 hour plus am/pm format
	if(time_as_structure->tm_hour <12)
	{ampm='A';}
	else
	{ampm='P';}
	hours_12=(time_as_structure->tm_hour)%12;
	if(hours_12 == 0) {hours_12=12;}
	
	printf("\nEE 3420 Initialization started at %02i/%02i/%4i, %02i:%02i:%02i %c.M.\n",
		(time_as_structure->tm_mon+1),
		time_as_structure->tm_mday,
		(time_as_structure->tm_year+1900),
		hours_12,
		time_as_structure->tm_min,
		time_as_structure->tm_sec,
		ampm	
	);
	
	//read system time as seconds since epoch on January 1, 1970
	time(&time_as_integer);
	end_time=time_as_integer;
	//format time as structure of human interest values
	time_as_structure = localtime(&time_as_integer);
	//convert 24-hour dormat into 12 hour plus am/pm format
	if(time_as_structure->tm_hour <12)
	{ampm='A';}
	else
	{ampm='P';}
	hours_12=(time_as_structure->tm_hour)%12;
	if(hours_12 == 0) {hours_12=12;}
	
	printf("EE 3420 Shutdown completed at %02i/%02i/%4i, %02i:%02i:%02i %c.M.\n",
		(time_as_structure->tm_mon+1),
		time_as_structure->tm_mday,
		(time_as_structure->tm_year+1900),
		hours_12,
		time_as_structure->tm_min,
		time_as_structure->tm_sec,
		ampm	
	);	
	
	printf("Program operation took %li seconds.\n",(end_time-start_time));

	return(tempvar);
}




	

#endif  //#ifndef EE3420_PI_H
	

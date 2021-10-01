/*
 * example2.ino
 * revision date: 30 May 2021
 * Written by: William Stapleton
 * The purpose of this program is to demonstrate the associated header file functions
 * for building a communications protocol with the Raspberry Pi 4 which will be
 * needed for the EE 3420 laboratory assignments in the Spring 2021 semester.
 * This code is designed for use on the Arduino Mega 2560.
 * The communication protocol is required for operating cooperatively with a Raspberry Pi 
 * with the Arduino Mega 2560 in the role of "smart peripheral".
 * The Arduino Mega will be operating with a complementary program filling this role.
 * The ee3420_mega.h header will need to be in the same directory as this file.
 */

/* the ee3420_mega.h header includes a lot of setup/shutdown functionality */
/* the header was written for this class and should be included in every lab program */
#include "ee3420_mega.h"



//keypad device in Rexqualis kit has pins from left to right ...
//row 0, row 1, row 2, row 3, col 0, col 1, col 2, col 3

// Keypad library described here: https://playground.arduino.cc/Code/Keypad
#include <Keypad.h>
//variables needed for Keypad
const byte keypad_rows = 4; //four rows
const byte keypad_cols = 4; //three columns
char keypad_keys[keypad_rows][keypad_cols] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

/*
 * //alternate keypad layout
 * char keypad_keys[keypad_rows][keypad_cols] = {
 *   {'1','2','3','A'},
 *   {'4','5','6','B'},
 *   {'7','8','9','C'},
 *   {'E','0','F','D'}
 * };
 */

//the following places the keypad on the end header of the Mega2560
byte keypad_rowPins[keypad_rows] = {22, 23, 24, 25}; //connect to the row pinouts of the keypad
byte keypad_colPins[keypad_cols] = {26, 27, 28, 29}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keypad_keys), keypad_rowPins, keypad_colPins, keypad_rows, keypad_cols );

char keypad_current_key;

//The Rexqualis kit contains a character-based Liquid Crystal Display (LCD) with 2 rows, 16 columns
int lcd_rows=2;
int lcd_cols=16;

//LiquidCrystal library is described here: https://www.arduino.cc/en/Reference/LiquidCrystal

#include <LiquidCrystal.h>
//LiquidCrystal pin options ...
/*
 * LiquidCrystal(rs, enable, d4, d5, d6, d7)
 * LiquidCrystal(rs, rw, enable, d4, d5, d6, d7)
 * LiquidCrystal(rs, enable, d0, d1, d2, d3, d4, d5, d6, d7)
 * LiquidCrystal(rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7) 
 */

//the following places the LCD on the end header of the Mega2560
//the rw pins MUST be grounded with this arrangement
//LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(30, 31, 32, 33, 34, 35);
//must include lcd.begin() in setup()

//needed for temporarily holding text to be sent to LCD
char lcd_text_string[40];
int lcd_current_row;
int lcd_current_col;
int LED_RED = 50; //PINS 50 - 53 For 220 Ohm LEDS
int LED_YELLOW = 51; 
int LED_GREEN = 52;
int LED_BLUE = 53;
int LED_BUZZER = 14;



void setup() {
  // put your setup code here, to run once:

  //initialize the serial port that will communicate to the Raspberry Pi over USB
  Serial.begin(115200, SERIAL_8N1);

  //initialize the pin controlling the built-in LED on the Arduino MEGA 2560
  //usually, this is equivalent to pin 13
  pinMode(LED_BUILTIN,OUTPUT);  //built in LED on Arduino Mega is pin 13
  digitalWrite(LED_BUILTIN,0);  //turn built in LED off

  pinMode(LED_RED,OUTPUT);  //built in LED on Arduino Mega is pin 13
  digitalWrite(LED_RED,0);  //turn built in LED off

  pinMode(LED_BUZZER,OUTPUT);  //built in LED on Arduino Mega is pin 13
  digitalWrite(LED_BUZZER,0);  //turn built in LED off

  pinMode(LED_GREEN,OUTPUT);  //built in LED on Arduino Mega is pin 13
  digitalWrite(LED_GREEN,0);  //turn built in LED off

  pinMode(LED_YELLOW,OUTPUT);  //built in LED on Arduino Mega is pin 13
  digitalWrite(LED_YELLOW,0);  //turn built in LED off

  pinMode(LED_BLUE,OUTPUT);  //built in LED on Arduino Mega is pin 13
  digitalWrite(LED_BLUE,0);  //turn built in LED off
    
  lcd.begin(lcd_cols,lcd_rows);  //needed to tell LiquidCrystal library the number of columns,rows on LCD

  //wait for the ENQ/ACK handshake to be initialized by the Raspberry Pi
  //function should be called in setup()
  //function defined in ee3420_mega.h
  serial_wait_for_enquire();  

  digitalWrite(LED_BUILTIN,1);  //turn built in LED on to indicate synchronization with remote microprocessor

  //initially clear the serial command receive buffer in case there was random data in the variables on startup
  //this should always be done before entering loop()
  clear_serial_receive_buffer();
   
}  //end of setup()

void loop() {
  // put your main code here, to run repeatedly:

  /* serial_build_command will build a command string one character at a time*/
  /* It will gether characters until a full command line is built */
  /* The function is non-blocking if no characters are available */
  /* Once a complete command is available, the global variable serial_received_instruction_complete is set */
  serial_build_command();
  
  /*only parse a command and try to execute once a complete command is available */
  if(serial_received_instruction_complete)
  {
    /*starting a new command, make a note that it isn't complete */
    serial_command_execution_complete=0;

    //in case this is a LCD:TEXT command, copy the string before parsing,
    //skipping the first 9 characters "LCD:TEXT:"
    strcpy(lcd_text_string, &serial_receive_buffer[9]);
  
    /*break command into parameters */
    parse_command_string(serial_receive_buffer, serial_command_parameters, parameter_separators);
    /*for ease of comparision, make parameters uniformly uppercase */
    make_command_parameters_uppercase(serial_command_parameters);

    /* what follows here should be a series of checks to interpret the parsed command and execute */
    /* successfule execution of a command should always set serial_command_execution_complete */

    // beginning of ID:TYPE
    if( !strcmp(&serial_command_parameters[0][0],"ID") &&
        !strcmp(&serial_command_parameters[1][0],"TYPE") 
      )
    {
      if(!strcmp(&serial_command_parameters[2][0],"?"))
      {
        Serial.write(ACK);
        Serial.print("ID:TYPE:");           //output the first part of the response without a newline
        Serial.println(my_device_type);     //complete the response and end with a newline
        serial_command_execution_complete=1;  //indicates completed command
      }
      else
      {
        strcpy(serial_device_type,&serial_command_parameters[2][0]);
        Serial.write(ACK);
        Serial.println("OK");
        serial_command_execution_complete=1;
      }
    }
    // end of ID:TYPE    

    // beginning of ID:NAME
    if( !strcmp(&serial_command_parameters[0][0],"ID") &&
        !strcmp(&serial_command_parameters[1][0],"NAME") 
      )
    {
      if(!strcmp(&serial_command_parameters[2][0],"?"))
      {
        Serial.write(ACK);
        Serial.print("ID:NAME:");
        Serial.println(my_device_name);
        serial_command_execution_complete=1;  //indicates completed command
      }
      else
      {
        strcpy(serial_device_name,&serial_command_parameters[2][0]);
        Serial.write(ACK);
        Serial.println("OK");
        serial_command_execution_complete=1;
      }
    }
    // end of ID:NAME 
    
    // check for ID with unknown parameter
    // must appear after all other options for ID
    if( !strcmp(&serial_command_parameters[0][0],"ID") &&
        (!serial_command_execution_complete) 
      )
    {
        Serial.write(NACK);
        Serial.print("ERROR:");
        Serial.println(ERROR_INVALID_PARAMETER);  //LED can be 0 (off) or 1 (on), any other value is invalid
        serial_command_execution_complete=1;  //indicates completed command
    }
    // end of check for ID with unknown parameter
    

    // beginning of LED:BUILTIN
    if( !strcmp(&serial_command_parameters[0][0],"LED") &&
        !strcmp(&serial_command_parameters[1][0],"BUILTIN") 
      )
    {
      if(!strcmp(&serial_command_parameters[2][0],"?"))  //request LED state
      {
        Serial.write(ACK);
        Serial.print("LED:BUILTIN:");
        Serial.println(digitalRead(LED_BUILTIN));
        serial_command_execution_complete=1;  //indicates completed command
      }
      else if(!strcmp(&serial_command_parameters[2][0],"0")) //turn LED off
      {
        Serial.write(ACK);
        digitalWrite(LED_BUILTIN,0);
        Serial.println("OK");
        serial_command_execution_complete=1;  //indicates completed command
      }
      else if(!strcmp(&serial_command_parameters[2][0],"1"))  //turn LED on
      {
        Serial.write(ACK);
        digitalWrite(LED_BUILTIN,1);
        Serial.println("OK");
        serial_command_execution_complete=1;  //indicates completed command
      }
      else                                    // something else
      {
        Serial.write(NACK);
        Serial.print("ERROR:");
        Serial.println(ERROR_INVALID_DATA);  //LED can be 0 (off) or 1 (on), any other value is invalid
        serial_command_execution_complete=1;  //indicates completed command
      }
    }
    // end of LED:BUILTIN

    // beginning of LED:BUZZER
    if( !strcmp(&serial_command_parameters[0][0],"LED") &&
        !strcmp(&serial_command_parameters[1][0],"BUZZER") 
      )
    {
      if(!strcmp(&serial_command_parameters[2][0],"?"))  //request LED state
      {
        Serial.write(ACK);
        Serial.print("LED:BUZZER:");
        Serial.println(digitalRead(LED_BUZZER));
        serial_command_execution_complete=1;  //indicates completed command
      }
      else if(!strcmp(&serial_command_parameters[2][0],"0")) //turn LED off
      {
        Serial.write(ACK);
        digitalWrite(LED_BUZZER,0);
        Serial.println("OK");
        serial_command_execution_complete=1;  //indicates completed command
      }
      else if(!strcmp(&serial_command_parameters[2][0],"1"))  //turn LED on
      {
        Serial.write(ACK);
        digitalWrite(LED_BUZZER,1);
        Serial.println("OK");
        serial_command_execution_complete=1;  //indicates completed command
      }
      else                                    // something else
      {
        Serial.write(NACK);
        Serial.print("ERROR:");
        Serial.println(ERROR_INVALID_DATA);  //LED can be 0 (off) or 1 (on), any other value is invalid
        serial_command_execution_complete=1;  //indicates completed command
      }
    }
    // end of LED:BUZZER
    
    // beginning of LED:RED
    if( !strcmp(&serial_command_parameters[0][0],"LED") &&
        !strcmp(&serial_command_parameters[1][0],"RED") 
      )
    {
      if(!strcmp(&serial_command_parameters[2][0],"?"))  //request LED state
      {
        Serial.write(ACK);
        Serial.print("LED:RED:");
        Serial.println(digitalRead(LED_RED));
        serial_command_execution_complete=1;  //indicates completed command
      }
      else if(!strcmp(&serial_command_parameters[2][0],"0")) //turn LED off
      {
        Serial.write(ACK);
        digitalWrite(LED_RED,0);
        Serial.println("OK");
        serial_command_execution_complete=1;  //indicates completed command
      }
      else if(!strcmp(&serial_command_parameters[2][0],"1"))  //turn LED on
      {
        Serial.write(ACK);
        digitalWrite(LED_RED,1);
        Serial.println("OK");
        serial_command_execution_complete=1;  //indicates completed command
      }
      else                                    // something else
      {
        Serial.write(NACK);
        Serial.print("ERROR:");
        Serial.println(ERROR_INVALID_DATA);  //LED can be 0 (off) or 1 (on), any other value is invalid
        serial_command_execution_complete=1;  //indicates completed command
      }
    }
    // end of LED:RED

    // beginning of LED:YELLOW
    if( !strcmp(&serial_command_parameters[0][0],"LED") &&
        !strcmp(&serial_command_parameters[1][0],"YELLOW") 
      )
    {
      if(!strcmp(&serial_command_parameters[2][0],"?"))  //request LED state
      {
        Serial.write(ACK);
        Serial.print("LED:YELLOW:");
        Serial.println(digitalRead(LED_YELLOW));
        serial_command_execution_complete=1;  //indicates completed command
      }
      else if(!strcmp(&serial_command_parameters[2][0],"0")) //turn LED off
      {
        Serial.write(ACK);
        digitalWrite(LED_YELLOW,0);
        Serial.println("OK");
        serial_command_execution_complete=1;  //indicates completed command
      }
      else if(!strcmp(&serial_command_parameters[2][0],"1"))  //turn LED on
      {
        Serial.write(ACK);
        digitalWrite(LED_YELLOW,1);
        Serial.println("OK");
        serial_command_execution_complete=1;  //indicates completed command
      }
      else                                    // something else
      {
        Serial.write(NACK);
        Serial.print("ERROR:");
        Serial.println(ERROR_INVALID_DATA);  //LED can be 0 (off) or 1 (on), any other value is invalid
        serial_command_execution_complete=1;  //indicates completed command
      }
    }
    // end of LED:YELLOW

    // beginning of LED:GREEN
    if( !strcmp(&serial_command_parameters[0][0],"LED") &&
        !strcmp(&serial_command_parameters[1][0],"GREEN") 
      )
    {
      if(!strcmp(&serial_command_parameters[2][0],"?"))  //request LED state
      {
        Serial.write(ACK);
        Serial.print("LED:GREEN:");
        Serial.println(digitalRead(LED_GREEN));
        serial_command_execution_complete=1;  //indicates completed command
      }
      else if(!strcmp(&serial_command_parameters[2][0],"0")) //turn LED off
      {
        Serial.write(ACK);
        digitalWrite(LED_GREEN,0);
        Serial.println("OK");
        serial_command_execution_complete=1;  //indicates completed command
      }
      else if(!strcmp(&serial_command_parameters[2][0],"1"))  //turn LED on
      {
        Serial.write(ACK);
        digitalWrite(LED_GREEN,1);
        Serial.println("OK");
        serial_command_execution_complete=1;  //indicates completed command
      }
      else                                    // something else
      {
        Serial.write(NACK);
        Serial.print("ERROR:");
        Serial.println(ERROR_INVALID_DATA);  //LED can be 0 (off) or 1 (on), any other value is invalid
        serial_command_execution_complete=1;  //indicates completed command
      }
    }
    // end of LED:GREEN

    // beginning of LED:BLUE
    if( !strcmp(&serial_command_parameters[0][0],"LED") &&
        !strcmp(&serial_command_parameters[1][0],"BLUE") 
      )
    {
      if(!strcmp(&serial_command_parameters[2][0],"?"))  //request LED state
      {
        Serial.write(ACK);
        Serial.print("LED:BLUE:");
        Serial.println(digitalRead(LED_BLUE));
        serial_command_execution_complete=1;  //indicates completed command
      }
      else if(!strcmp(&serial_command_parameters[2][0],"0")) //turn LED off
      {
        Serial.write(ACK);
        digitalWrite(LED_BLUE,0);
        Serial.println("OK");
        serial_command_execution_complete=1;  //indicates completed command
      }
      else if(!strcmp(&serial_command_parameters[2][0],"1"))  //turn LED on
      {
        Serial.write(ACK);
        digitalWrite(LED_BLUE,1);
        Serial.println("OK");
        serial_command_execution_complete=1;  //indicates completed command
      }
      else                                    // something else
      {
        Serial.write(NACK);
        Serial.print("ERROR:");
        Serial.println(ERROR_INVALID_DATA);  //LED can be 0 (off) or 1 (on), any other value is invalid
        serial_command_execution_complete=1;  //indicates completed command
      }
    }
    // end of LED:BLUE

    // check for LED with unknown parameter
    // must appear after all other options for LED
    if( !strcmp(&serial_command_parameters[0][0],"LED") &&
        (!serial_command_execution_complete) 
      )
    {
        Serial.write(NACK);
        Serial.print("ERROR:");
        Serial.println(ERROR_INVALID_PARAMETER);  //LED can be 0 (off) or 1 (on), any other value is invalid
        serial_command_execution_complete=1;  //indicates completed command
    }
    // end of check for LED with unknown parameter

    // beginning of KEYPAD
    //KEYPAD:? is acceptable
    if( !strcmp(&serial_command_parameters[0][0],"KEYPAD") )
    {
      if(!strcmp(&serial_command_parameters[1][0],"?"))
      {
        Serial.write(ACK);
        Serial.print("KEYPAD:");           //output the first part of the response without a newline

        keypad_current_key = keypad.getKey();
        if (keypad_current_key != NO_KEY)
        {
          Serial.println(keypad_current_key);
        }
        else
        {
          Serial.println("NONE");
        }
        
        serial_command_execution_complete=1;  //indicates completed command
      }
    }
    // end of KEYPAD 

    // check for KEYPAD with unknown parameter
    // must appear after all other options for KEYPAD
    if( !strcmp(&serial_command_parameters[0][0],"KEYPAD") &&
        (!serial_command_execution_complete) 
      )
    {
        Serial.write(NACK);
        Serial.print("ERROR:");
        Serial.println(ERROR_INVALID_PARAMETER);  //LED can be 0 (off) or 1 (on), any other value is invalid
        serial_command_execution_complete=1;  //indicates completed command
    }
    // end of check for KEYPAD with unknown parameter

    // beginning of LCD
    //LCD:CLEAR is acceptable
    //LCD:CURSOR:<row>:<col> is acceptable
    //LCD:TEXT:<character(s)> is acceptable
    
    if( !strcmp(&serial_command_parameters[0][0],"LCD") )
    {
      if(!strcmp(&serial_command_parameters[1][0],"CLEAR"))
      {
        lcd.clear();
        Serial.write(ACK);
        Serial.println("OK");           //output the first part of the response without a newline
        serial_command_execution_complete=1;  //indicates completed command
      } //end of if(!strcmp(&serial_command_parameters[1][0],"CLEAR"))
      
      if(!strcmp(&serial_command_parameters[1][0],"CURSOR"))
      {
        lcd_current_row = atoi(&serial_command_parameters[2][0]);
        lcd_current_col = atoi(&serial_command_parameters[3][0]);
        if( lcd_current_row < 0 || 
            lcd_current_row >= lcd_rows ||
            lcd_current_col < 0 || 
            lcd_current_col >= lcd_cols 
          )
        {
          Serial.write(NACK);
          Serial.print("ERROR:");
          Serial.println(ERROR_INVALID_DATA);  
          serial_command_execution_complete=1;  //indicates completed command          
        }
        else
        {
          lcd.setCursor(lcd_current_col, lcd_current_row);
          Serial.write(ACK);
           Serial.println("OK");           //output the first part of the response without a newline
         serial_command_execution_complete=1;  //indicates completed command 
        }
      }  //end of if(!strcmp(&serial_command_parameters[1][0],"CURSOR"))

      if(!strcmp(&serial_command_parameters[1][0],"TEXT"))
      {
        lcd.print(lcd_text_string);
        Serial.write(ACK);
        Serial.println("OK");           //output the first part of the response without a newline
        serial_command_execution_complete=1;  //indicates completed command 
      }  //end of if(!strcmp(&serial_command_parameters[1][0],"TEXT"))
      
    }  //end of if( !strcmp(&serial_command_parameters[0][0],"LCD") )
    // end of LCD    
        
    // check for LCD with unknown parameter
    // must appear after all other options for LCD
    if( !strcmp(&serial_command_parameters[0][0],"LCD") &&
        (!serial_command_execution_complete) 
      )
    {
        Serial.write(NACK);
        Serial.print("ERROR:");
        Serial.println(ERROR_INVALID_PARAMETER);  //LED can be 0 (off) or 1 (on), any other value is invalid
        serial_command_execution_complete=1;  //indicates completed command
    }
    // end of check for LCD with unknown parameter

/*************************************************************************************************/
/* PUT YOUR FUNCTIONS HERE AND KEEP THE MAIN STRUCTURE INTACT FOR BEST RESULTS                   */
/*************************************************************************************************/

    /* If this point is reached without executng a command, return an error message */
    if(!serial_command_execution_complete)
    {
      Serial.write(NACK);
      Serial.print("ERROR:");
      Serial.println(ERROR_UNKNOWN_COMMAND);
      serial_command_execution_complete=1;
    }

    // clear the buffer and clear serial_received_instruction_complete to prepare for next command */
    clear_serial_receive_buffer();
  }  //end of if(serial_received_instruction_complete)

  /*
   * What follows is a template for including periodic actions in the main loop 
   * that does not require blocking "busy" loop.
   * This example program does not require any specific periodic actions but this is
   * a common enough need that thie template code is included here.
   */

   int now;
   int last_action_time;
   int delay_duration=1000;  //this can be whatever amunt of time you want

   now=millis();  //reads the system time in milliseconds
   if( (now-last_action_time) > delay_duration)
   {
    //here is where you do the periodic action
    last_action_time=now;  //be sure to update the last_action_time when you act
   }
  
}  //end of loop()

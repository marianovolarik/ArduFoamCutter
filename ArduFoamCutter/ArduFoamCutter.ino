#include "settings.h"
#include "gcode_commands.h"

extern const int x_step_pin;
extern const int y_step_pin;
extern const int a_step_pin;
extern const int z_step_pin;
extern const int x_dir_pin;
extern const int y_dir_pin;
extern const int a_dir_pin;
extern const int z_dir_pin;


void setup()
{

  Serial.begin(baudrate);
  Serial.setTimeout(1000);

  pinMode(x_step_pin, OUTPUT);
  pinMode(y_step_pin, OUTPUT);
  pinMode(a_step_pin, OUTPUT);
  pinMode(z_step_pin, OUTPUT);

  pinMode(x_dir_pin, OUTPUT);
  pinMode(y_dir_pin, OUTPUT);
  pinMode(a_dir_pin, OUTPUT);
  pinMode(z_dir_pin, OUTPUT);

  // Initialize Timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCNT1  = 0;

  OCR1A = 38;            // compare match register 16MHz/256/2Hz

  /* Setear prescaler a 8 -> 16 MHz/8 -> 2 MHz -> resolución 0.5 us */
  /* Periodo us = 2*<value> */
  TCCR1B = 0;
  TCCR1B |= (1 << WGM12);   /* CTC MODE: Clear Time on Compare */
  TCCR1B |= (1 << CS12);    /* Clock source -> prescaler:256 */

  TIMSK1 |= (1 << OCIE1A);  /* Enable timer compare interrupt */
  interrupts();             // enable all interrupts
}



/* Serial input buffer */
const unsigned int buffer_len = 60;
char buffer[buffer_len];
unsigned char buffer_index = 0;


void loop()
{
  char c = 0;
  char command = 0;
  int g_command = 0;
  int len = 0;

  while (Serial.available() > 0){
    len = Serial.readBytesUntil('\n',  buffer, 60);
    if (len){
      buffer[len] = '\0';
      command = buffer[0];
      if (command == 'G'){
        g_command = atoi(buffer + 1);
        switch (g_command){

          /* Linear */
          case 0: {
            command_G0(buffer);
            break;
          }

          /* Absolute mode */
          case 90: {
            command_G90(buffer);
            break;
          }

          /* Relative mode */
          case 91: {
            command_G91(buffer);
            break;
          }

        }
      }
    }
  }
}

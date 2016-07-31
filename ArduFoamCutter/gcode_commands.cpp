#include "Arduino.h"
#include "settings.h"
#include "gcode_commands.h"

/* Responses */
const char response_ok[] = "ok";

typedef struct{
  long delta_x,
       delta_y,
       delta_a,
       delta_z;
  long longest_delta;
  long epsilon_x,
       epsilon_y,
       epsilon_a,
       epsilon_z;
  long remaining_steps;
  char dir_mask;
} bresenham_line;

bresenham_line l;

/* Mode absolute or relative */
boolean absolute_mode = true;

/* Current position */
float x_pos;
float y_pos;
float a_pos;
float z_pos;


void bresenham_line_create(float x, float y, float a, float z){

  float m1 = 0, m2 = 0;

  noInterrupts();

  l.dir_mask = 0x0;
  if (x > 0) l.dir_mask |= DIR_MASK_X;
  if (y > 0) l.dir_mask |= DIR_MASK_Y;
  if (z > 0) l.dir_mask |= DIR_MASK_Z;
  if (a > 0) l.dir_mask |= DIR_MASK_A;

  l.delta_x = abs(x)/MM_PER_STEP_H;
  l.delta_y = abs(y)/MM_PER_STEP_V;
  l.delta_z = abs(z)/MM_PER_STEP_H;
  l.delta_a = abs(a)/MM_PER_STEP_V;

  l.epsilon_x = 0;
  l.epsilon_y = 0;
  l.epsilon_z = 0;
  l.epsilon_a = 0;

  m1 = max(l.delta_x, l.delta_y);
  m2 = max(l.delta_a, l.delta_z);
  l.longest_delta = max(m1, m2);

  l.remaining_steps = l.longest_delta;

  interrupts();
}


/*
 * If <parameter> occurs in <parameters> returns the floating value after
 * the first ocurrence of <paramenter>.
 * If <parameter> doesn't occur in <parameters> returns <default_value>.
 *
 * For example:
 *  Suppose <parameters> = G00 X3.14 Y2 X5 then
 *    get_parameter_value(parameters, 'X', 0) returns 3.14
 *    get_parameter_value(parameters, 'Y', 0) returns 2
 *    get_parameter_value(parameters, 'Z', -1) returns -1
 */
float get_parameter_value(char *parameters, char parameter, float default_value)
{
  char *parameter_pos = NULL;
  float result = 0;

  parameter_pos = strchr(parameters, parameter);
  result = parameter_pos != NULL ? atof(parameter_pos + 1) : default_value;

  return result;
}


/*
 * 'G0 X<x_value> Y<y_value> A<a_value> Z<z_value>'
 * Not all parameters are mandatory, at least one must be explicited.
 * 
 * If relative mode is set:
 *  G0 will move the axis(ies) the amount specified by <_value>.
 *
 * If absolute mode is set:
 *  G0 will move the axis(ies) up to reach the position specified by <_value>.
 *
 * The amount are in milimeters.
 *
 * For example:
 * If relative mode is set:
 * G0 X5.75 Y-2.875
 *  It will moves the horizontal-left axis 5.75mm forward and
 *  the vertical-left axis 2.875 mm down, forming a downward diagonal.
 *  If the values are greater than zero, the will move forward, else backward.
 *
 * If absolute mode is set, and the current position is X = 4 and Y = 5,
 * G0 X5.75 Y-2.875
 *  It will move the horizontal-left axis forward 1.75 mm (from 4 to 5.75) and
 *  the vertical-left axis 7.875 mm down (from 5 to -2.875).
 */
void command_G0(char *parameters)
{
  float x_value = absolute_mode ? x_pos : 0;
  float y_value = absolute_mode ? y_pos : 0;
  float a_value = absolute_mode ? a_pos : 0;
  float z_value = absolute_mode ? z_pos : 0;
  float feedrate = 0;


  /* Get parameter values */
  x_value = get_parameter_value(parameters, 'X', x_value);
  y_value = get_parameter_value(parameters, 'Y', y_value);
  a_value = get_parameter_value(parameters, 'A', a_value);
  z_value = get_parameter_value(parameters, 'Z', z_value);

  bresenham_line_create(x_value - (absolute_mode ? x_pos : 0),
                        y_value - (absolute_mode ? y_pos : 0),
                        a_value - (absolute_mode ? a_pos : 0),
                        z_value - (absolute_mode ? z_pos : 0));
}


ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{

  char step_mask = 0;

  if (l.remaining_steps == 0)
    return;

  l.epsilon_x += l.delta_x;
  if (2*l.epsilon_x >= l.longest_delta){
    l.epsilon_x -= l.longest_delta;
    step_mask |= STEP_MASK_X;
  }

  l.epsilon_y += l.delta_y;
  if (2*l.epsilon_y >= l.longest_delta){
    l.epsilon_y -= l.longest_delta;
    step_mask |= STEP_MASK_Y;
  }

  l.epsilon_a += l.delta_a;
  if (2*l.epsilon_a >= l.longest_delta){
    l.epsilon_a -= l.longest_delta;
    step_mask |= STEP_MASK_A;
  }

  l.epsilon_z += l.delta_z;
  if (2*l.epsilon_z >= l.longest_delta){
    l.epsilon_z -= l.longest_delta;
    step_mask |= STEP_MASK_Z;
  }

  digitalWrite(x_dir_pin, (l.dir_mask & DIR_MASK_X) ? FORWARD : BACKWARD);
  digitalWrite(y_dir_pin, (l.dir_mask & DIR_MASK_Y) ? FORWARD : BACKWARD);
  digitalWrite(a_dir_pin, (l.dir_mask & DIR_MASK_A) ? FORWARD : BACKWARD);
  digitalWrite(z_dir_pin, (l.dir_mask & DIR_MASK_Z) ? FORWARD : BACKWARD);

  // According to A4988 datasheet (tC: setup time >= 200 ns)
  delayMicroseconds(1);

  if (step_mask & STEP_MASK_X){
    digitalWrite(x_step_pin, HIGH);
    if (l.dir_mask & DIR_MASK_X){
      x_pos += MM_PER_STEP_H;
    }else{
      x_pos -= MM_PER_STEP_H;
    }
  }

  if (step_mask & STEP_MASK_Y){
    digitalWrite(y_step_pin, HIGH);
    if (l.dir_mask & DIR_MASK_Y){
      y_pos += MM_PER_STEP_V;
    }else{
      y_pos -= MM_PER_STEP_V;
    }
  }

  if (step_mask & STEP_MASK_A){
    digitalWrite(a_step_pin, HIGH);
    if (l.dir_mask & DIR_MASK_A){
      a_pos += MM_PER_STEP_H;
    }else{
      a_pos -= MM_PER_STEP_H;
    }
  }

  if (step_mask & STEP_MASK_Z){
    digitalWrite(z_step_pin, HIGH);
    if (l.dir_mask & DIR_MASK_Z){
      z_pos += MM_PER_STEP_V;
    }else{
      z_pos -= MM_PER_STEP_V;
    }
  }

  delayMicroseconds(1);

  digitalWrite(x_step_pin, LOW);
  digitalWrite(y_step_pin, LOW);
  digitalWrite(a_step_pin, LOW);
  digitalWrite(z_step_pin, LOW);

  l.remaining_steps--;
  if (l.remaining_steps == 0){
    Serial.print(response_ok);
  }
}


/*
 * 'G90'
 * No parameters.
 *
 * Sets to absolute mode.
 */
void command_G90(char *parameters)
{
  absolute_mode = true;
  Serial.print(response_ok);

}


/*
 * 'G91'
 * No parameters.
 *
 * Sets to relative mode.
 */
void command_G91(char *parameters)
{
  absolute_mode = false;
  Serial.print(response_ok);
}
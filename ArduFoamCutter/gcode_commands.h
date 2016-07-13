#ifndef GCODE_COMMANDS_H
#define GCODE_COMMANDS_H


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
void command_G0(char *parameters);


/*
 * 'G90'
 * No parameters.
 *
 * Sets to absolute mode.
 */
void command_G90(char *parameters);


/*
 * 'G91'
 * No parameters.
 *
 * Sets to relative mode.
 */
void command_G91(char *parameters);

#endif

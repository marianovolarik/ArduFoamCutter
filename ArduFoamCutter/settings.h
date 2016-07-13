#ifndef SETTINGS_H
#define SETTINGS_H

#define FORWARD   LOW
#define BACKWARD  !FORWARD

#define STEP_MASK_X 0x1
#define STEP_MASK_Y 0x2
#define STEP_MASK_A 0x4
#define STEP_MASK_Z 0x8

#define DIR_MASK_X 0x1
#define DIR_MASK_Y 0x2
#define DIR_MASK_A 0x4
#define DIR_MASK_Z 0x8

/*
 * Pin assignment
 */
const int x_dir_pin = 2;  // Horizontal-left axis
const int x_step_pin = 3;

const int y_dir_pin = 4;  // Vertical-left axis
const int y_step_pin = 5;

const int a_dir_pin = 8;  // Horizontal-right axis
const int a_step_pin = 9;

const int z_dir_pin = 10; // Vertical-right axis
const int z_step_pin = 11;

/*
 * Sets the amount of milimeters that it advances in a single step
 */
#define MM_PER_STEP_H (float) 0.0015625 // Horizontal
#define MM_PER_STEP_V (float) 0.0015625 // Vertical

/*
 * Serial baudrate
 */
const long baudrate = 115200;

#endif

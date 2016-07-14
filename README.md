# ArduFoamCutter

## Firmware ##
The firmware runs on [Arduino Nano board](https://www.arduino.cc/en/Main/ArduinoBoardNano). It is
controlled by a serial port. At this moment supports the following commands (they must be *newline* ended):

+ **G0** **X**x **Y**y **A**a **Z**z: moves synchronously the motors
	+ to the position (x, y, a, z), if *absolute mode* is set, or,
	+ by a quantity of x, y, a and z, if *relative mode* is set.
+ **G90** Sets to *absolute mode.*
+ **G91** Sets to *relative mode*.

The firmware implements a fast version of [Bresenham's line algorithm](https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm) to move synchronously the motors and it uses a internal timer to generate
pulses (it never block, does not uses not `delayMicroseconds()`).

### Axes naming ###
The axes are named as follows:

+ X: Horizontal-Left.
+ Y: Vertical-Left.
+ A: Horizontal-Right.
+ Z: Vertical-Right.

### Pin assignment ###
`settings.h` defines the out pins assigment for the `dir` and `step` signal of each [A4988 Pololu driver](https://www.pololu.com/product/1182). By default:

	const int x_dir_pin = 2;
	const int x_step_pin = 3;
	
	const int y_dir_pin = 4;
	const int y_step_pin = 5;
	
	const int a_dir_pin = 8;
	const int a_step_pin = 9;
	
	const int z_dir_pin = 10;
	const int z_step_pin = 11;
	



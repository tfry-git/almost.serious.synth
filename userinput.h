/** Abstraction layer for user input hardware */

#ifndef USERINPUT_H
#define USERINPUT_H

// Enable this block in oder to use a potentiometer or joystick (one axis is enough) as up/down input.
//#define POT_AS_TUNER
//#define POT_AS_TUNER_PIN_UD PA0   // Input pin
//#define POT_AS_TUNER_PIN_LR PA1   // Input pin
//#define POT_AS_TUNER_THRESHOLD 100  // Offsets this close to center will be read as "0"
//#define POT_AS_TUNER_FAST_THRESHOLD 1000  // Above this offset, acceleration increases a lot.
//#define POT_AS_TUNER_LSHIFT 18   // resolution. Probably to be left untouched
#define USER_INPUT_HIGHER_RES 2  // You probably want this: Gives only slightly lower granularity at higher values

// Input via 4 by 4 button matrix
//#define BUTTON_MATRIX_INPUT

#define TOUCHSCREEN_INPUT
#define TOUCHSCREEN_INPUT_ACCEL 5
#define TOUCHSCREEN_INPUT_ACCEL_TIMEOUT 2000
#define TOUCHSCREEN_INPUT_REPEAT_TIMEOUT 500

///////////////////// You should not need to modify anything below this line /////////////////////////////

#if ((defined POT_AS_TUNER) + (defined BUTTON_MATRIX_INPUT) + (2 * defined(TOUCHSCREEN_INPUT))) != 2
#error Please define exactly only one up/down and one button input device in __FILE__
#endif

#include <Arduino.h>
void setup_updown ();
int8_t read_updown ();
int8_t read_leftright ();

int read_keypad();
void setup_keypad();

#if defined(TOUCHSCREEN_INPUT)
void setup_touchpad();
void touchpad_getPoint(int *x, int *y, int *z);
#endif

#endif

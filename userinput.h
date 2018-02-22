/** Abstraction layer for user input hardware */

#ifndef USERINPUT_H
#define USERINPUT_H

// Enable this block of defines in order to use a (quadarture) rotary encoder as up/down input.
// #define ENCODER_AS_TUNER
// #define ENCODER_PIN_A PA1    // Input pins. NOTE That these _must_ support intterupts, _and_ interrupts must not interfere with other pins
// #define ENCODER_PIN_B PA2    // // (e.g. on STM32F103C8T6, PAx will interfere with PBx)
// #define BOUNCE_TIMEOUT 2     // ms
// #define USER_INPUT_HIGHER_RES 0 // You probably want this: Gives lower granularity at higher values

// Enable this block in oder to use a potentiometer or joystick (one axis is enough) as up/down input.
#define POT_AS_TUNER
#define POT_AS_TUNER_PIN_UD PA0   // Input pin
#define POT_AS_TUNER_PIN_LR PA1   // Input pin
#define POT_AS_TUNER_THRESHOLD 100  // Offsets this close to center will be read as "0"
#define POT_AS_TUNER_FAST_THRESHOLD 1000  // Above this offset, acceleration increases a lot.
#define POT_AS_TUNER_LSHIFT 18   // resolution. Probably to be left untouched
#define USER_INPUT_HIGHER_RES 2  // You probably want this: Gives only slightly lower granularity at higher values

///////////////////// You should not need to modify anything below this line /////////////////////////////

#if ((defined POT_AS_TUNER) + defined (ENCODER_AS_TUNER)) != 1
#error Please define exactly only one up/down input device in userinput.h
#endif

#include <Arduino.h>
void setup_updown ();
int8_t read_updown ();
int8_t read_leftright ();

#endif

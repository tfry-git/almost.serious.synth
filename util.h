#ifndef UTIL_H
#define UTIL_H

#include <Arduino.h>

/** Assorted hardware independent utility functions */

/** Safe conversion from int to uint8_t */
inline uint8_t u8ranged (int input) {
  if (input < 0) return 0;
  if (input > 255) return 255;
  return input;
}

/* cheap itoa implementation, right aligned to width digits. My stdlib.h does not provide one, and sprintf and friends consume way too much flash
 *  If width digits is not enough, the start of the string is chopped (but at least this function will not write before the start of buf). */
void cheap_itoa (char *buf, int16_t value, byte width);

#endif

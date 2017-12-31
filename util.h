#ifndef UTIL_H
#define UTIL_H

/** Assorted hardware independent utility functions */

/** Safe conversion from int to uint8_t */
uint8_t u8ranged (int input) {
  if (input < 0) return 0;
  if (input > 255) return 255;
  return input;
}

/* cheap itoa implementation, right aligned to width digits. My stdlib.h does not provide one, and sprintf and friends consume way too much flash
 *  If width digits is not enough, the start of the string is chopped (but at least this function will not write before the start of buf). */
void cheap_itoa (char *buf, int16_t value, byte width) {
  bool neg = false;
  buf[width] = '\0';
  if (value < 0) {
    neg = true;
    value = -value;
  } else if (value == 0) {
    buf[--width] = '0';  // special case: don't print empty string for plain 0
  }
  while (value > 0 && width > 0) {
    buf[--width] = '0' + value % 10;
    value = value / 10;
  }
  if (neg && width > 0) {
    buf[--width] = '-';
  }
  while (width > 0) {
    buf[--width] = ' ';
  }
}

#endif

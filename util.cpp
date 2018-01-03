#include "util.h"

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


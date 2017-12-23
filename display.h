/*
Display setup. I'm using an SSD1306 128*64. Anything smaller, and you'll have a hard time fitting everything to the screen. */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306_STM32.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

/* cheap itoa implementation, right aligned to width digits. My stdlib.h does not provide one, and sprintf and friends consume way too much flash
If width digits is not enough, the start of the string is chopped (but at least this function will not write before the start of buf).
*/
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

void display_icon(int8_t row, int8_t col, const char *shortname, const char* value) {
    display.fillRect(col*32-6, row*13, 32, 13, 0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(col*32,row*13);
    display.println(shortname);
    display.setCursor(col*32+24-(strlen(value)*6),row*13+6);
    display.println(value);
//    display.display();  // Always followed by a call to display_detail in our sketch, so not needed.
}

void display_detail(const char *label, const char* value) {
    display.fillRect(0, 57, 128, 7, 0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,57);
    display.println(label);
    display.setCursor(128-strlen(value)*6,57);
    display.println(value);
    display.display();
}

void setup_display()   {                
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the correct I2C addr
  display.clearDisplay ();
  display_detail ("Starting", "");
}

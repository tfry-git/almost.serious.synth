#ifndef DISPLAY_H
#define DISPLAY_H

/*
Display setup. I'm using an SSD1306 128*64. Anything smaller, and you'll have a hard time fitting everything to the screen. */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306_STM32.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

void display_icon(int8_t row, int8_t col, const char *shortname, const char* value, bool active) {
    display.fillRect(col*32-6, row*13, 32, 13, active ? 1 : 0);
    display.setTextSize(1);
    display.setTextColor(active ? BLACK : WHITE);
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

#endif


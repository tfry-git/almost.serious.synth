/*
Display setup. I'm using an SSD1306 128*64. Anything smaller, and you'll have a hard time fitting everything to the screen. */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306_STM32.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define SECTION_WIDTH 32
#define SECTION_HEIGHT 13
#define FONT_SPACING 6

void display_clear() {
    display.clearDisplay();
}

void display_commit() {
    display.display();
}

void display_icon(int8_t row, int8_t col, const char *shortname, const char* value, bool active) {
    display.fillRect(col*SECTION_WIDTH-FONT_SPACING, row*SECTION_HEIGHT, SECTION_WIDTH, SECTION_HEIGHT, active ? 1 : 0);
    display.setTextSize(1);
    display.setTextColor(active ? BLACK : WHITE);
    display.setCursor(col*SECTION_WIDTH,row*SECTION_HEIGHT);
    display.println(shortname);
    display.setCursor(col*SECTION_WIDTH+(4-strlen(value))*FONT_SPACING,row*SECTION_HEIGHT+FONT_SPACING);
    display.println(value);
//    display.display();  // Always followed by a call to display_detail in our sketch, so not needed.
}

void display_button (int8_t row, int8_t col, const char *name) {
    // TODO: we should improve on this...
    display_icon (row, col, "----", name, false);
}

void display_detail(const char *label, const char* value) {
    display.fillRect(0, 57, 128, 7, 0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,57);
    display.println(label);
    display.setCursor(128-strlen(value)*FONT_SPACING,57);
    display.println(value);
    display.display();
}

void display_line(const char *line, int8_t row) {
  int16_t x = 0;
  int16_t y = row * SECTION_HEIGHT + 1;
  int16_t w = 127;
  int16_t h = SECTION_HEIGHT - 2;
  display.setTextSize(1);
  display.fillRect(x, y, w, h, 0);
  display.setTextColor(WHITE);
  display.setCursor (x+1, y+2);
  display.println(line);
}

// Draw a (menu) header. Note: x, y, w, and h given in logical sections, i.e. 1..4
void display_header_bar(const char *label, int8_t row) {
  int16_t x = 0;
  int16_t y = row * SECTION_HEIGHT + 1;
  int16_t w = 127;
  int16_t h = SECTION_HEIGHT - 2;
  display.setTextSize(1);
  display.fillRect(x, y, w, h, 1);
  display.setTextColor(BLACK);
  display.setCursor (x+1, y+2);
  display.println(label);
}

void setup_display()   {                
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the correct I2C addr
  display.clearDisplay ();
  display_detail ("Starting", "");
}


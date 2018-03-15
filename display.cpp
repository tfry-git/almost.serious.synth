/*
Display setup. I'm using an SSD1306 128*64. Anything smaller, and you'll have a hard time fitting everything to the screen. */

#include <Wire.h>
#include <Adafruit_GFX.h>

#define DISPLAY_ILI9341   // ILI9341-based color display 320*240
//#define DISPLAY_SSD1306    // SSD1306-based b/w display 128*64

#ifdef DISPLAY_SSD1306
    #include <Adafruit_SSD1306_STM32.h>

    #define OLED_RESET 4
    Adafruit_SSD1306 display(OLED_RESET);

    #define SECTION_WIDTH 32
    #define SECTION_HEIGHT 13
    #define FONT_SPACING 6
    #define SECTION_SPACING FONT_SPACING
    #define TEXT_SIZE 1
    #define SECTION_OFFSETX 0
    #define SECTION_OFFSETY 0
    #define DETAILS_OFFSETY 57
    #define DETAILS_WIDTH 128
    #define DETAILS_HEIGHT 7

    #define BG_COLOR 0
    #define FG_COLOR 1
    #define SECTION_BG_ACTIVE FG_COLOR
    #define SECTION_BG_INACTIVE BG_COLOR
    #define SECTION_FG_ACTIVE BG_COLOR
    #define SECTION_FG_INACTIVE FG_COLOR
    #define SECTION_FG_OUTLINE SECTION_FG_ACTIVE
    #define SET_MAIN_FONT() {}
    #define FONT_OFFSET() (0u)
    void display_printBottomRightAligned(uint16_t x, uint16t_y, const char* value) {
        display.setCursor(x-strlen(value)*FONT_SPACING, y-FONT_SPACING);
        display.println(value);
    }
    void display_printTopLeftAligned(uint16_t x, uint16_t_y, const char* value) {
        display.setCursor(x, y);
        display.println(value);
    }
#elif defined(DISPLAY_ILI9341)
    #include <Adafruit_ILI9341_STM.h>

    #define TFT_CS         PB4
    #define TFT_DC         PA15
    #define TFT_RST        PB3
    Adafruit_ILI9341_STM display(TFT_CS, TFT_DC, TFT_RST); // SPI1

    #define SECTION_WIDTH ((uint16_t) 75)
    #define SECTION_HEIGHT ((uint16_t) 45)
    #define SECTION_SPACING 5
    #define TEXT_SIZE 1
    #define SECTION_OFFSETX ((uint16_t) 0)
    #define SECTION_OFFSETY ((uint16_t) 30)
    #define DETAILS_OFFSETY 220
    #define DETAILS_WIDTH (SECTION_WIDTH*4-SECTION_SPACING)
    #define DETAILS_HEIGHT 19

    #define BG_COLOR 0
    #define FG_COLOR display.color565(255,255,255)
    #define SECTION_BG_ACTIVE display.color565(20,50,50)
    #define SECTION_BG_INACTIVE display.color565(20,20,20)
    #define SECTION_FG_ACTIVE display.color565(255,125,125)
    #define SECTION_FG_INACTIVE display.color565(255,255,255)
    #define SECTION_FG_OUTLINE SECTION_FG_ACTIVE

    #include <Fonts/FreeSans9pt7b.h>
    #define SET_MAIN_FONT() display.setFont(&FreeSans9pt7b)
    #define FONT_OFFSET() (11)   // not the same as yAdvance, unfortunately.
    void display_printBottomRightAligned(uint16_t x, uint16_t y, const char* value) {
        int16_t bx, by;
       uint16_t bw, bh;
       display.getTextBounds(const_cast<char*> (value), 10, 10, &bx, &by, &bw, &bh);  // NOTE: cannot use x, here, as that may "draw" out of the display bounds, returning garbage.
       display.setCursor(x - bw, y);
       display.println(value);
    }
    void display_printTopLeftAligned(uint16_t x, uint16_t y, const char* value) {
       display.setCursor(x, y+FONT_OFFSET());
       display.println(value);
    }
#endif

void display_clear() {
#ifdef DISPLAY_SSD1306
    display.clearDisplay();
#elif defined(DISPLAY_ILI9341)
    display.fillRect(0, 0, 320, 240, BG_COLOR);
#endif
}

void display_commit() {
#ifdef DISPLAY_SSD1306
    display.display();
#endif
}

Struct Dimensions {
    uint16_t x, y, w, h;
};
Dimensions sectionDimensions(int8_t row, int8_t col) {
    Dimensions ret;
    ret.x = SECTION_OFFSETX + col*SECTION_WIDTH;
    ret.y = SECTION_OFFSETY + row*SECTION_HEIGHT;
    ret.w = SECTION_WIDTH - SECTION_SPACING;
    ret.h = SECTION_HEIGHT - SECTION_SPACING;
}

void display_icon(int8_t row, int8_t col, const char *shortname, const char* value, bool active) {
    Dimensions dims = sectionDimensions (row, col);
    display.drawRect(dims.x, dims.y, dims.w, dims.h, SECTION_FG_OUTLINE);
    display.fillRect(dims.x+1, dims.y+1, dims.w-2, dims.h-2, active ? SECTION_BG_ACTIVE : SECTION_BG_INACTIVE);
    if (col != 0) display.fillRect(dims.x-SECTION_SPACING, dims.y, SECTION_SPACING, dims.h, BG_COLOR);
    display.setTextColor(active ? SECTION_FG_ACTIVE : SECTION_FG_INACTIVE);
    display_printTopLeftAligned(dims.x + 2, dims.y + 2, shortname);
    display_printBottomRightAligned(dims.x + dims.w - 2, dims.y + dims.h - 2, value);
//    display.display();  // Always followed by a call to display_detail in our sketch, so not needed.
}

void display_button (int8_t row, int8_t col, const char *name) {
    // TODO: we should improve on this...
    display_icon (row, col, "----", name, false);
}

void display_detail(const char *label, const char* value) {
    display.fillRect(0, DETAILS_OFFSETY, DETAILS_WIDTH, DETAILS_HEIGHT, BG_COLOR);
    display.setTextColor(FG_COLOR);
    display_printTopLeftAligned(1, DETAILS_OFFSETY+1, label);
    display_printBottomRightAligned(DETAILS_WIDTH-1, DETAILS_OFFSETY+FONT_OFFSET()+1, value);
    display.println(value);
    display_commit();
}

void display_line(const char *line, int8_t row) {
  int16_t x = SECTION_OFFSETX;
  int16_t y = SECTION_OFFSETY + row * SECTION_HEIGHT + 1;
  int16_t w = SECTION_WIDTH*4;
  int16_t h = SECTION_HEIGHT - 2;
  display.fillRect(x, y, w, h, BG_COLOR);
  display.setTextColor(FG_COLOR);
  display_printTopLeftAligned(x+1, y+2, line);
}

// Draw a (menu) header. Note: x, y, w, and h given in logical sections, i.e. 1..4
void display_header_bar(const char *label, int8_t row) {
  int16_t x = SECTION_OFFSETX;
  int16_t y = SECTION_OFFSETY + row * SECTION_HEIGHT + 1;
  int16_t w = SECTION_WIDTH*4;
  int16_t h = SECTION_HEIGHT - 2;
  display.fillRect(x, y, w, h, SECTION_BG_ACTIVE);
  display.setTextColor(SECTION_FG_ACTIVE);
  display_printTopLeftAligned(x+1, y+2, label);
}

void setup_display()   {
#ifdef DISPLAY_SSD1306
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the correct I2C addr
#elif defined(DISPLAY_ILI9341)
  display.begin();
  display.setRotation(1);
#endif
  display_clear();
  SET_MAIN_FONT();
  display.setTextSize(TEXT_SIZE);
  display_detail ("Starting", "");
}


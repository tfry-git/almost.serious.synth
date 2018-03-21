/*
Display setup. I'm using an SSD1306 128*64. Anything smaller, and you'll have a hard time fitting everything to the screen. */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "userinput.h"

/************** Hardware setup section. You may need to adjust this to your needs *****************************/

    // variant 1: ILI9341 8 bit parallel interface
    #include <Adafruit_TFTLCD_8bit_STM32.h>
    #define DISPLAY_320_240_COLOR
    Adafruit_TFTLCD_8bit_STM32 display;
    void init_display() {
        display.reset();
        display.begin(display.readID());
        display.setRotation(1);
    }
    void display_commit() {}
    void display_pause() {}
    void display_resume() {}

/*    // variant 2: ILI 9341 SPI interface.
    #include <Adafruit_ILI9341_STM.h>
    #define DISPLAY_320_240_COLOR
    #define TFT_CS         PB4
    #define TFT_DC         PA15
    #define TFT_RST        PB3
    Adafruit_ILI9341_STM display(TFT_CS, TFT_DC, TFT_RST);
    SPIClass spi1(1);
    void init_display() {
        display.begin(spi1);
        display.setRotation(1);
    }
    void display_commit() {}
    void display_pause() {  // Needed, if display shares SPI bus with another device
        display.endTransaction();
    }
    void display_resume() {
        display.beginTransaction();
    }
*/

/*    // variant 3: SSD1306 128*64 bw display I2C
    #include <Adafruit_SSD1306_STM32.h>

    #define OLED_RESET 4
    Adafruit_SSD1306 display(OLED_RESET);
    #define DISPLAY_128_64_BW
    void init_display() {
        display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    }
    void display_commit() {
        display.display();
    }
    void display_pause() {}
    void display_resume() {}
*/

/************** End hardware setup section. You should not need to adjust anything below this *****************/

#ifdef DISPLAY_128_64_BW
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
    #define MAXX 127
    #define MAXY 63

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
        display.print(value);
    }
    void display_printTopLeftAligned(uint16_t x, uint16_t_y, const char* value) {
        display.setCursor(x, y);
        display.print(value);
    }
#elif defined(DISPLAY_320_240_COLOR)
    #define SECTION_WIDTH ((uint16_t) 75)
    #define SECTION_HEIGHT ((uint16_t) 45)
    #define SECTION_SPACING 5
    #define TEXT_SIZE 1
    #define SECTION_OFFSETX ((uint16_t) 0)
    #define SECTION_OFFSETY ((uint16_t) 30)
    #define DETAILS_OFFSETY 220
    #define DETAILS_WIDTH (SECTION_WIDTH*4-SECTION_SPACING)
    #define DETAILS_HEIGHT 19
    #define HEADER_HEIGHT 17
    #define MAXX 319
    #define MAXY 239

    #define BG_COLOR 0
    #define FG_COLOR display.color565(255,255,255)
    #define SECTION_BG_ACTIVE display.color565(20,50,50)
    #define SECTION_BG_INACTIVE display.color565(20,20,20)
    #define SECTION_FG_ACTIVE display.color565(255,255,255)
    #define SECTION_FG_INACTIVE display.color565(255,125,125)
    #define SECTION_FG_OUTLINE SECTION_FG_ACTIVE

    #include <Fonts/FreeSans9pt7b.h>
    #define SET_MAIN_FONT() display.setFont(&FreeSans9pt7b)
    #define FONT_OFFSET() (11)   // not the same as yAdvance, unfortunately.
    void display_printBottomRightAligned(uint16_t x, uint16_t y, const char* value) {
        int16_t bx, by;
       uint16_t bw, bh;
       display.getTextBounds(const_cast<char*> (value), 10, 10, &bx, &by, &bw, &bh);  // NOTE: cannot use x, here, as that may "draw" out of the display bounds, returning garbage.
       display.setCursor(x - bw, y);
       display.print(value);
    }
    void display_printTopLeftAligned(uint16_t x, uint16_t y, const char* value) {
       display.setCursor(x, y+FONT_OFFSET());
       display.print(value);
    }
#endif

struct Dimensions {
    Dimensions(uint16_t _x, uint16_t _y, uint16_t _w, uint16_t _h) : x(_x), y(_y), w(_w), h(_h) {};
    Dimensions() {};
    uint16_t x, y, w, h;
    bool contains(uint16_t px, uint16_t py) {
        return (px >= x && px <= (x + w) && py >= y && py <= (y + h));
    }
};

void draw_icon_box(Dimensions &dims, bool active) {
    display.drawRect(dims.x, dims.y, dims.w, dims.h, SECTION_FG_OUTLINE);
    display.fillRect(dims.x+1, dims.y+1, dims.w-2, dims.h-2, active ? SECTION_BG_ACTIVE : SECTION_BG_INACTIVE);
}

inline Dimensions lrBarDimensions() {
    return Dimensions(0, 0, DETAILS_WIDTH, HEADER_HEIGHT);
}

inline Dimensions lrButtonDimensions(bool left) {
    Dimensions ret = lrBarDimensions();
    if (!left) ret.x += ret.w - 20 + SECTION_SPACING;
    ret.w = 20 - SECTION_SPACING;
    return ret;
}

void display_page_header(const char* label, bool lr_buttons) {
#ifdef DISPLAY_SSD1306
    // ignore
#else
    display.setTextColor(SECTION_FG_ACTIVE);
    display_printTopLeftAligned(20, 2, label);
#ifdef TOUCHSCREEN_INPUT
    if (lr_buttons) {
        Dimensions dims = lrButtonDimensions(true);
        draw_icon_box(dims, false);
        display_printTopLeftAligned(dims.x + 2, dims.y + 2, "<");
        dims = lrButtonDimensions(false);
        draw_icon_box(dims, false);
        display_printTopLeftAligned(dims.x + 2, dims.y + 2, ">");
    }
#endif
#endif
}

inline Dimensions udBarDimensions() {
    return Dimensions(DETAILS_WIDTH + SECTION_SPACING, HEADER_HEIGHT, MAXX - DETAILS_WIDTH - SECTION_SPACING, MAXY-HEADER_HEIGHT);
}

inline Dimensions udButtonDimensions(bool up) {
    Dimensions ret = udBarDimensions();
    if (!up) ret.y += ret.h - 20;
    ret.h = 20;
    return ret;
}

// Draw up / down scrollbar, with the current position given between 0 (bottom) and 1 << 15 (top)
void display_ud_bar(int16_t min, int16_t max, int16_t current_step, bool show) {
#ifdef TOUCHSCREEN_INPUT
    Dimensions fulldims = udBarDimensions();
// TODO
    if (show) {
        display.setTextColor(SECTION_FG_ACTIVE);
        Dimensions dims = udButtonDimensions(true);
        draw_icon_box(dims, false);
        display_printTopLeftAligned(dims.x + 4, dims.y + 2, "+");
        dims = udButtonDimensions(false);
        draw_icon_box(dims, false);
        display_printTopLeftAligned(dims.x + 4, dims.y + 2, "-");
    } else {
        display.fillRect(fulldims.x, fulldims.y + 21, fulldims.w, fulldims.h - 42, BG_COLOR);
    }

    display.drawLine(fulldims.x + (fulldims.w/2), fulldims.y + 21, fulldims.x + (fulldims.w/2), fulldims.y + fulldims.h - 21, SECTION_FG_ACTIVE);
    uint16_t pos = ((uint32_t) fulldims.h - 44) * (current_step - min) / (max - min);
    display.drawRect(fulldims.x, fulldims.y + fulldims.h - 22 - pos, fulldims.w, 2, SECTION_FG_INACTIVE);
#endif
}

Dimensions sectionDimensions(int8_t row, int8_t col) {
    Dimensions ret;
    ret.x = SECTION_OFFSETX + col*SECTION_WIDTH;
    ret.y = SECTION_OFFSETY + row*SECTION_HEIGHT;
    ret.w = SECTION_WIDTH - SECTION_SPACING;
    ret.h = SECTION_HEIGHT - SECTION_SPACING;
    return ret;
}

void display_icon(int8_t row, int8_t col, const char *shortname, const char* value, bool active) {
    Dimensions dims = sectionDimensions (row, col);
    draw_icon_box(dims, active);
    if (col != 0) display.fillRect(dims.x-SECTION_SPACING, dims.y, SECTION_SPACING, dims.h, BG_COLOR);
    display.setTextColor(active ? SECTION_FG_ACTIVE : SECTION_FG_INACTIVE);
    display_printTopLeftAligned(dims.x + 2, dims.y + 2, shortname);
    display_printBottomRightAligned(dims.x + dims.w - 2, dims.y + dims.h - 2, value);
}

void display_text(int8_t row, int8_t col, const char* value) {
    Dimensions dims = sectionDimensions (row, col);
    display.setTextColor(SECTION_FG_ACTIVE);
    display_printBottomRightAligned(dims.x + dims.w - 2, dims.y + dims.h - 2, value);
}

void display_button (int8_t row, int8_t col, const char *name) {
    // TODO: we should improve on this...
    display_icon (row, col, "----", name, false);
}

#ifdef TOUCHSCREEN_INPUT
void display_detail(const char *label, const char* value);
class TouchScreenButtonHandler {
public:
    TouchScreenButtonHandler() : button(NoButton), numbutton(-1) {};
    void update() {
        button = NoButton;
        display_pause();
        int x, y, z;
        touchpad_getPoint(&x, &y, &z);
        display_resume();
        if (z) {
            //display.drawPixel(x, y, display.color565(0, 255, 255));
            if (udBarDimensions().contains(x, y)) {
                if (udButtonDimensions(true).contains(x, y)) {
                    button = UpButton;
                } else if (udButtonDimensions(false).contains(x, y)) {
                    button = DownButton;
                }
            } else if (lrBarDimensions().contains(x, y)) {
                if (lrButtonDimensions(true).contains(x, y)) {
                    button = LeftButton;
                } else if (lrButtonDimensions(false).contains(x, y)) {
                    button = RightButton;
                }
            } else  {
                for (uint8_t i = 0; i < 16; ++i) {
                    if (sectionDimensions(i / 4, i % 4).contains(x, y)) {
                        button = SectionButton;
                        numbutton = i;
                        break;
                    }
                }
            }
        }
        if (!z) {  // More reliable than button == NoButton (jitter)
            pressed_since = 0;
            tick_sent = false;
        } else if ((button != NoButton) && (!pressed_since)) {
            pressed_since = millis();
        }
    }
    int8_t read_updown() {
        if ((button != UpButton) && (button != DownButton)) return 0;
        uint32_t now = millis();
        if (tick_sent && ((now - pressed_since) < TOUCHSCREEN_INPUT_REPEAT_TIMEOUT)) return 0;

        int8_t ret;
        if (now - pressed_since > TOUCHSCREEN_INPUT_ACCEL_TIMEOUT) {
            ret = (button == DownButton) ? -TOUCHSCREEN_INPUT_ACCEL : TOUCHSCREEN_INPUT_ACCEL;
        } else {
            ret = (button == DownButton) ? -1 : 1;
        }
        tick_sent = true;
        return ret;
    }
    int8_t read_leftright() {
        if ((button != LeftButton) && (button != RightButton)) return 0;
        if (tick_sent) return 0;  // no key repeat for left/right

        tick_sent = true;
        if (button == LeftButton) {
            return -1;
        }
        return 1;
    }
    int8_t read_key() {
        if (button != SectionButton) return -1;
        if (tick_sent) return -1;
        tick_sent = true;
        return numbutton;
    }
private:
    enum Button {
        UpButton, DownButton, LeftButton, RightButton, SectionButton, NoButton
    } button;
    int8_t numbutton;
    bool tick_sent;
    uint32_t pressed_since;
} ts_button_handler;

int8_t read_updown() {
    ts_button_handler.update();
    return ts_button_handler.read_updown();
}

int8_t read_leftright() {
    return ts_button_handler.read_leftright();
}

void setup_keypad() {
}

int read_keypad() {
   return ts_button_handler.read_key();
}

void setup_updown () {
    display_pause();
    setup_touchpad();
    display_resume();
}
#endif

void display_clear() {
    display.fillScreen(BG_COLOR);
}

void display_detail(const char *label, const char* value) {
    display.fillRect(0, DETAILS_OFFSETY, DETAILS_WIDTH, DETAILS_HEIGHT, BG_COLOR);
    display.setTextColor(FG_COLOR);
    display_printTopLeftAligned(1, DETAILS_OFFSETY+1, label);
    display_printBottomRightAligned(DETAILS_WIDTH-1, DETAILS_OFFSETY+FONT_OFFSET()+1, value);
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
  init_display();
  display_clear();
  SET_MAIN_FONT();
  display.setTextSize(TEXT_SIZE);
  display_detail ("Starting", "");
}


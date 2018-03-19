#include "userinput.h"

#ifdef TOUCHSCREEN_INPUT

/** This file is a basic abstraction layer for handling the touchscreen. Some examples are given on 
 *  implementing alternative setups.
 *  
 *  The more interesting touchscreen related functions are inside display.cpp (as they need to know
 *  the geometry of on-screen buttons). */

// Default config: 4 Wire resistive touch screen.
#include <TouchScreen_STM32.h>
#define XM PA1 // TFT_WR
#define YP PA2 // TFT_RS
#define XP PB7 // TFT_D7
#define YM PB6 // TFT_D6
TouchScreen ts = TouchScreen(YM, XM, YP, XP);

void setup_touchpad() {
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 850
#define TS_MAXY 850
    ts.rangeSet(320,240);
    ts.setCalibrationParameters(150, 850, 150, 850);
}

void touchpad_getPoint(int *x, int *y, int *z) {
    TSPoint p;
    if (ts.getPoint(&p)) {
        *x = p.x;
        *y = p.y;
    }
    *z = p.z;
}

/*  // Alternative config example: XPT2046-based SPI touchscreen controller on SPI2
#include <XPT2046_touch.h>
SPIClass spi2(2);
#define CS_PIN PB5
XPT2046_touch ts(CS_PIN, spi2);

#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 4000
#define TS_MAXY 4010

void setup_touchpad() {
    ts.begin();
}

void touchpad_getPoint(int *x, int *y, int *z) {
    TS_Point p = ts.getPoint();
    if (p.z) {
        *x = map(x, TS_MAXX, TS_MINX, 0, display.width());
        *y = map(y, TS_MAXY, TS_MINY, 0, display.height());
    }
    *z = p.z;
}
*/

#endif


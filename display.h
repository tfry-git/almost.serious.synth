#ifndef DISPLAY_H
#define DISPLAY_H

/* Display abstraction layer. See/adjust display.cpp for implementation details.

The screen layout is built around the notion of 4 x 4 "areas", representing icons, buttons, or settings,
and one "detail" area (could also be an overlay, though).
*/

void setup_display();
void display_clear();
void display_commit();

void display_icon(int8_t row, int8_t col, const char *shortname, const char* value, bool active);
/** Display a short message in the detail area. */
void display_detail(const char *label, const char* value);
// Draw a (menu) header. Note: x, y, w, and h given in logical sections, i.e. 1..4
void display_header_bar(const char *label, int8_t row);

#endif


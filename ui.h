#ifndef UI_H
#define UI_H
/** This file contains the structures and logic for making the UI work. */

#include "synthsettings.h"

/** Base class for all "pages" / screens of the UI */
class UIPage {
public:
  virtual void handleUpDown (int8_t delta) = 0;   /// handle encoder input (+/-)
  virtual void handleButton (int8_t but) = 0;  /// handle button (matrix) press
  virtual void updateDisplay () = 0;           /// update display (if necessary)
  enum PageID {
    MenuPage1,
    SynthSettingsPage1
  };
  static void setCurrentPage (PageID page);
  static UIPage* currentPage () { return current_page; };
protected:
  virtual void initDisplay () = 0;             /// intialize the display. To be called, when the page has just been activated.
  void drawMenuOption (const char* label, int8_t row, int8_t col);
  static UIPage* current_page;
};

/** UI specification for the (a) synthesizer settings screen. In essence, button
 * press selects the setting to tune, encoder ticks change the setting value. */
class SynthSettingsPage : public UIPage {
  void drawIconForSetting (uint8_t setting, bool active);
  void initDisplay () override;
  void handleUpDown (int8_t delta) override; 
  void updateDisplay () override;
  void handleButton (int8_t button) override;
private:
  uint8_t current_setting = NothingSetting;
  bool update = false;
};

/** UI specification for the main menu. Currently. there is exactly one main menu page, but more might be added in the future, or it might be made scrollable. */
class MenuPage : public UIPage {
public:
  void initDisplay () override;
  void handleUpDown (int8_t delta) override;
  void handleButton (int8_t but);
  void updateDisplay ();
private:
  void drawMIDIPlayOptions ();
};

#endif

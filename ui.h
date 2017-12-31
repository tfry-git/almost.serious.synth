#ifndef UI_H
#define UI_H
/** This file contains the structures and logic for making the UI work. */

#include "synthsettings.h"

/** Base class for all "pages" / screens of the UI */
class UIPage {
public:
  virtual void initDisplay () = 0;             /// intialize the display. To be called, when the page has just been activated.
  virtual void handleEnc (int8_t delta) = 0;   /// handle encoder input (+/-)
  virtual void handleButton (int8_t but) = 0;  /// handle button (matrix) press
  virtual void updateDisplay () = 0;           /// update display (if necessary)
};
UIPage *current_page;

/** UI specification for the (a) synthesizer settings screen. In essence, button
 * press selects the setting to tune, encoder ticks change the setting value. */
class SynthSettingsPage : public UIPage {
  void drawIconForSetting (uint8_t setting, bool active) {
    if (setting == NothingSetting) return;
    display_icon (setting / 4, setting % 4, settings[setting].shortname, settings[setting].displayValue(), active);
  }

  void initDisplay () override {
    for (uint8_t i = 0; i < SETTINGS_COUNT-1; ++i) {
      drawIconForSetting (i, i == current_setting);
    }
  }

  void handleEnc (int8_t delta) override {
    if (delta != 0) {
      Setting& setting = settings[current_setting];
      update = true;
      int mult = setting.value / setting.dynamic_res + 1;
      setting.value += delta * mult;
      if (setting.value < setting.min) setting.value = setting.min;
      if (setting.value > setting.max) setting.value = setting.max;
    }
  }
 
  void updateDisplay () override {
    if (update) {
      drawIconForSetting (current_setting, true);
      Setting setting = settings[current_setting];
      display_detail(setting.name, setting.displayValue());
      update = false;
    }
  }

  void handleButton (int8_t button) override {
    if (button < 0) return;
    if (button > NothingSetting) return;
    if (button == NothingSetting) {
      if (noteDelay.ready ()) {
        MyHandleNoteOn (1, rand (20) + 77, 100);
        noteDelay.start ();
        saveVoice ();
      }
    } else {
      update = update || (current_setting != button);
      drawIconForSetting (current_setting, false);
      current_setting = button;
    }
  }
private:
  uint8_t current_setting = NothingSetting;
  bool update = false;
};
SynthSettingsPage synth_settings_page_1;

#endif

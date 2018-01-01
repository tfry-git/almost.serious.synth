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
protected:
  void drawMenuOption (const char* label, int8_t row, int8_t col) {
    display_icon (row, col, "->", label, false);
  }
};
UIPage *current_page;
UIPage *menu_page_1p;  // Dirty HACK, TODO remove this.

void setCurrentPage (UIPage *newpage) {
  current_page = newpage;
  display.clearDisplay ();
  newpage->initDisplay();
  display.display();
}

/** UI specification for the (a) synthesizer settings screen. In essence, button
 * press selects the setting to tune, encoder ticks change the setting value. */
class SynthSettingsPage : public UIPage {
  void drawIconForSetting (uint8_t setting, bool active) {
    if (setting == NothingSetting) {
      drawMenuOption ("Menu", setting / 4, setting % 4);
    } else {
      display_icon (setting / 4, setting % 4, settings[setting].shortname, settings[setting].displayValue(), active);
    }
  }

  void initDisplay () override {
    for (uint8_t i = 0; i < SETTINGS_COUNT; ++i) {
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
      setCurrentPage (menu_page_1p);
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
SynthSettingsPage synth_settings_page_1;  // NOTE: could allocate / deallocate as needed, but there is not a whole lot of storage required per instance.

/** UI specification for the main menu. Currently. there is exactly one main menu page, but more might be added in the future, or it might be made scrollable. */
class MenuPage : public UIPage {
public:
  void initDisplay () override {
    display_header_bar ("Synth voice", 0);
    drawMenuOption ("Edit", 1, 0);
    drawMenuOption ("Save", 1, 2);
    drawMenuOption ("Load", 1, 3);
    display_header_bar ("MIDI", 2);
    drawMIDIPlayOptions ();
    drawMenuOption ("Save", 3, 2);
    drawMenuOption ("Load", 3, 3);
//    display_detail ("I am a placeholder", "");
  }
  void handleEnc (int8_t delta) override {
    // TODO: scrolling (once necessary)
  }
  void handleButton (int8_t but) {
    // TODO
    switch (but) {
      case 4:
        setCurrentPage (&synth_settings_page_1);
        break;
      case 6:
        saveVoice();
        display_detail("Voice", "saved");
        break;
      case 7:
        loadVoice();
        display_detail("Voice", "loaded");
        break;
      case 13:
        if (player.isPlaying ()) {
          player.stop ();
          drawMIDIPlayOptions ();
          display_detail ("Playback", "stopped");
        } else {
          player.play ();
          drawMIDIPlayOptions ();
          display_detail ("Random notes, for now...", "");
        }
        break;
      case 12:
        if (player.isRecording ()) {
          player.stop ();
          drawMIDIPlayOptions ();
          display_detail ("Recording", "stopped");
        } else {
          player.record ();
          drawMIDIPlayOptions ();
          display_detail ("Recording", "started");
        }
        break;
      case 14:
      case 15:
        display_detail ("Not yet implemented", ":-(");
    }
  }
  void updateDisplay () {
    // Not needed
  }
private:
  void drawMIDIPlayOptions () {
    if (player.isPlaying ()) {
      drawMenuOption ("Stop", 3, 1);
    } else {
      drawMenuOption ("Play", 3, 1);
    }
    if (player.isRecording ()) {
      drawMenuOption ("Stop", 3, 0);
    } else {
      drawMenuOption ("Rec", 3, 0);
    }
  }
};
MenuPage menu_page1;

#endif

#include "ui.h"
#include "synthsettings.h"
#include "display.h"
#include "userinput.h"
#include "midiplayer.h"

void UIPage::drawMenuOption (const char* label, int8_t row, int8_t col) {
  display_icon (row, col, "->", label, false);
}

UIPage *UIPage::current_page;

void SynthSettingsPage::drawIconForSetting (uint8_t setting, bool active) {
  if (setting == NothingSetting) {
    drawMenuOption ("Menu", setting / 4, setting % 4);
  } else {
    display_icon (setting / 4, setting % 4, settings[setting].shortname, settings[setting].displayValue(), active);
  }
}

void SynthSettingsPage::initDisplay () {
  for (uint8_t i = 0; i < SETTINGS_COUNT; ++i) {
    drawIconForSetting (i, i == current_setting);
  }
}

void SynthSettingsPage::handleUpDown (int8_t delta) {
  if (delta != 0) {
    Setting& setting = settings[current_setting];
    update = true;
    int mult = ((setting.value / setting.dynamic_res) >> USER_INPUT_HIGHER_RES) + 1;
    setting.value += delta * mult;
    if (setting.value < setting.min) setting.value = setting.min;
    if (setting.value > setting.max) setting.value = setting.max;
  }
}

void SynthSettingsPage::updateDisplay () {
  if (update) {
    drawIconForSetting (current_setting, true);
    Setting setting = settings[current_setting];
    display_detail(setting.name, setting.displayValue());
    update = false;
  }
}

void SynthSettingsPage::handleButton (int8_t button) {
  if (button < 0) return;
  if (button > NothingSetting) return;
  if (button == NothingSetting) {
    setCurrentPage (MenuPage1);
  } else {
    update = update || (current_setting != button);
    drawIconForSetting (current_setting, false);
    current_setting = button;
  }
}

void MenuPage::initDisplay () {
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

void MenuPage::handleUpDown (int8_t delta) {
  // TODO: scrolling (once necessary)
}

void MenuPage::handleButton (int8_t but) {
  switch (but) {
    case 4:
      setCurrentPage (SynthSettingsPage1);
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

void MenuPage::updateDisplay () {
  // Not needed
}

void MenuPage::drawMIDIPlayOptions () {
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

// These _could_ be allocated de-allocated dynamically, but they don't really take up much RAM
MenuPage menu_page1;
SynthSettingsPage synthsettings_page1;

void UIPage::setCurrentPage (UIPage::PageID page) {
  if (page == MenuPage1) {
    current_page = &menu_page1;
  } else {
    current_page = &synthsettings_page1;
  }
  display_clear();
  current_page->initDisplay();
  display_commit();
}


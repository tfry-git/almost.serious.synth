/*  Almost Serious Synth
 *
 *  UI (menu) definitions
 *
 *  Copyright (c) 2017, 2018 Thomas Friedrichsmeier
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ui.h"
#include "synthsettings.h"
#include "display.h"
#include "userinput.h"
#include "midiplayer.h"

UIPage *UIPage::current_page;
// These _could_ be allocated de-allocated dynamically, but they don't really take up much RAM
MenuPage menu_page1;
SynthSettingsPage synthsettings_page1 (0);
SynthSettingsPage synthsettings_page2 (16);
SelectFilePage select_file_page;
SaveFilePage save_file_page;

SynthSettingsPage::SynthSettingsPage (uint8_t offset) {
  SynthSettingsPage::offset = offset;
}

void SynthSettingsPage::drawIconForSetting (uint8_t setting, bool active) {
  if (setting == NothingSetting) {
    display_button ((setting / 4) % 4, setting % 4, "Menu");
  } else {
    display_icon ((setting / 4) % 4, setting % 4, settings[setting].shortname, settings[setting].displayValue(), active);
  }
}

void SynthSettingsPage::initDisplay () {
  for (uint8_t i = offset; i < (offset + NothingSetting); ++i) {
    drawIconForSetting (i, i == current_setting);
  }
}

void SynthSettingsPage::handleUpDown (int8_t delta) {
  if (delta != 0) {
    Setting& setting = settings[current_setting];
    update = true;
    int mult = (abs (setting.value / setting.dynamic_res) >> USER_INPUT_HIGHER_RES) + 1;
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
    button += offset;
    update = update || (current_setting != button);
    drawIconForSetting (current_setting, false);
    current_setting = button;
  }
}

void MenuPage::initDisplay () {
  display_header_bar ("Synth voice", 0);
  display_button (1, 0, "Edit");
  display_button (1, 1, "Edi2");
  display_button (1, 2, "Save");
  display_button (1, 3, "Load");
  display_header_bar ("MIDI", 2);
  drawMIDIPlayOptions ();
  display_button (3, 2, "Save");
  display_button (3, 3, "Load");
}

void MenuPage::handleUpDown (int8_t delta) {
  // TODO: scrolling (once necessary)
}

void loadMIDIFile (const char *file) {
  UIPage::setCurrentPage (UIPage::MenuPage1);
  if (file[0] != '\0') {
    player.play (file);
  }
}

void MenuPage::handleButton (int8_t but) {
  switch (but) {
    case 4:
      setCurrentPage (SynthSettingsPage1);
      break;
    case 5:
      setCurrentPage (SynthSettingsPage2);
      break;
    case 6:
      save_file_page.saveFile (openVoiceDirectory (), "NAME", ".VOC", saveVoice);
      break;
    case 7:
      select_file_page.selectFile (openVoiceDirectory (), loadVoice);
      break;
    case 13:
      if (player.isPlaying ()) {
        player.stop ();
        drawMIDIPlayOptions ();
        display_detail ("Playback", "stopped");
      } else {
        player.play ();
        drawMIDIPlayOptions ();
        display_commit ();
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
      display_detail ("Not yet implemented", ":-(");
      break;
    case 15:
      select_file_page.selectFile (openMidiDirectory (), loadMIDIFile);
      break;
  }
}

void MenuPage::updateDisplay () {
  // Not needed
}

void MenuPage::drawMIDIPlayOptions () {
  if (player.isPlaying ()) {
    display_button (3, 1, "Stop");
  } else {
    display_button (3, 1, "Play");
  }
  if (player.isRecording ()) {
    display_button (3, 0, "Stop");
  } else {
    display_button (3, 0, "Rec");
  }
}

void UIPage::setCurrentPage (UIPage::PageID page) {
  if (page == MenuPage1) {
    current_page = &menu_page1;
  } else if (page == SelectExistingFile) {
    current_page = &select_file_page;
  } else if (page == SaveFile) {
    current_page = &save_file_page;
  } else if (page == SynthSettingsPage2) {
    current_page = &synthsettings_page2;
  } else {
    current_page = &synthsettings_page1;
  }
  display_clear();
  current_page->initDisplay();
  display_commit();
}

void SelectFilePage::initDisplay () {
  display_clear ();
  dir.rewindDirectory ();
  entry = File ();
  for (uint16_t i = 0; i < pos; ++i) {
    if (entry) entry.close ();
    entry = dir.openNextFile ();  // skip files, after scrolling
    if (!entry) {
      pos = 0;
      dir.rewindDirectory ();
      break;
    }
  }

  for (uint16_t i = 0; i < 4; ++i) {
    if (!entry) {
      display_button (i, 0, "Exit");
      display_icon (i, 3, "", "--------", false);
    } else {
      display_button (i, 0, "Load");
      display_icon (i, 3, "", getFileName (entry), false);
      entry.close ();
    }
    entry = dir.openNextFile ();
    if (!entry) break;
  }
  if (entry) entry.close ();

  display_detail ("Up/down to scroll", "");
}

void SelectFilePage::handleUpDown (int8_t delta) {
  if (delta != 0) {
    pos -= delta;
    if (pos < 0) pos = 0;
    initDisplay ();
  }
}

void SelectFilePage::handleButton (int8_t but) {
  dir.rewindDirectory ();
  entry = File ();
  if (but % 4 == 0) {
    for (uint16_t i = 0; i < pos + (but / 4); ++i) {
      if (entry) entry.close ();
      entry = dir.openNextFile ();  // skip files, after scrolling
    }
    dir.close ();
    callback (getFileName (entry));
  } else if (but % 4 == 3) {
    dir.close ();
    callback ("");
  }
}

void SelectFilePage::updateDisplay () {
}

void SelectFilePage::selectFile (File directory, void (*callback)(const char*)) {
  dir = directory;
  pos = 0;
  SelectFilePage::callback = callback;
  UIPage::setCurrentPage (UIPage::SelectExistingFile);
}


void SaveFilePage::initDisplay () {
  display_clear ();
  char buf[15] = "........";
  buf[pos] = '!';
  display_line (buf, 0);
  strncpy (buf, name, 8);
  for (int i = strlen (buf); i < 8; ++i) {
    buf[i] = ' ';
  }
  buf[9] = '\0';
  strcat (buf, extension);
  display_line (buf, 1);
  display_icon (2, 1, "<", "", false);
  display_icon (2, 2, ">", "", false);
  display_button (3, 0, "Save");
  display_button (3, 3, "Cancel");
  display_detail ("Enter save file name", "");
}

void SaveFilePage::handleUpDown (int8_t delta) {
  if (delta == 0) return;

  char c = name[pos];
  if (delta < 0) {
    if (c == 'A') c = '9';
    else if (c == '0') c = ' ';
    else if (c == ' ') c = 'Z';
    else --c;
  } else {
    if (c == 'Z') c = ' ';
    else if (c == ' ') c = '0';
    else if (c == '9') c = 'A';
    else ++c;
  }
  name[pos] = c;

  initDisplay ();
}

void SaveFilePage::handleButton (int8_t but) {
  if (but == 9 && pos > 0) {  // left
    --pos;
    initDisplay ();
  } else if (but == 10 && pos < 7) { // right
    ++pos;
    initDisplay ();
  } else if (but == 12) {  // save
    char buf[13];
    strcpy (buf, name);
    strcat (buf, extension);
    callback (buf);
  } else if (but == 15) {  // cancel
    callback ("");
  }
}

void SaveFilePage::updateDisplay () {
}

void SaveFilePage::saveFile (File directory, const char* initial_name, const char *ext, void (*callback)(const char*)) {
  pos = 0;
  strncpy (name, initial_name, 8);
  strncpy (extension, ext, 4);
  SaveFilePage::callback = callback;
  setCurrentPage (UIPage::SaveFile);
}


#include "wavetables.h"
#include "util.h"
#include "synthsettings.h"

char printf_buf[8];
const char* Setting::displayValue() {
  if (value_labels) {
    return value_labels[value];
  }
  cheap_itoa (printf_buf, value, 5);
  return printf_buf;
}

const char* EFFECTS[] = {"LPFC", "LPFR", "LPFA", "OMix", "O1Fr", "Freq"};
Setting settings[SETTINGS_COUNT] = {
  {"Attack (ms)", "A", 0, 100, 10000, 5, NULL},
  {"Decay (ms)", "D", 0, 100, 10000, 5, NULL},
  {"Sustain (ms) ", "S", 0, 2000, 10000, 5, NULL},
  {"Release (ms)", "R", 0, 100, 10000, 5, NULL},
  {"Osc1 Waveform", "W", 0, 1, NUM_WAVEFORMS-1, 100, TABLE_NAMES},
  {"Osc Mix", "X", 0, 100, 255, 10, NULL},
  {"Osc2 Waveform", "W", 0, 1, NUM_WAVEFORMS-1, 100, TABLE_NAMES},
  {"Osc2 Tune halfst.", "T", -40, 0, 40, 100, NULL},
  {"LFO Waveform", "W", 0, 1, NUM_WAVEFORMS-1, 100, TABLE_NAMES},
  {"LF0 Freq * 200", "F", 1, 200, 16000, 5, NULL},
  {"LF0 Amp", "A", 0, 0, 255, 10, NULL},
  {"LF0 Effect", "X", 0, 0, 5, 100, EFFECTS },
  {"LPF Cutoff Freq", "C", 0, 100, 255, 10, NULL},
  {"LPF Resonance", "R", 0, 50, 170, 5, NULL},
  {"LPF Amp", "A", 0, 0, 255, 10, NULL},
  {"Nothing", "-", 0, 0, 255, 10, NULL}
};

#include "storage.h"
#include "ui.h"
#include "display.h"

void saveVoice (const char *name) {
  if (name[0] == '\0') return;
  File f = openVoiceFile (name);
  f.seek (0);
  int16_t vals[SETTINGS_COUNT];
  for (int i = 0; i < SETTINGS_COUNT; ++i) {
    int16_t val = settings[i].value;
    f.write ((byte) (val >> 8));
    f.write ((byte) (val & 0xFF));
  }
  f.close ();
  UIPage::setCurrentPage (UIPage::SynthSettingsPage1);
  display_detail ("Saved voice", f.name ());
}

void loadVoice (const char* name) {
  File f = openVoiceFile (name);
  f.seek (0);
  byte buf[SETTINGS_COUNT * 2];
  int len = f.read (buf, SETTINGS_COUNT * 2) / 2;
  for (int i = 0; i < len; ++i) {
    settings[i].value = (buf[i*2] << 8) | buf[i*2+1];
  }
  f.close ();
  UIPage::setCurrentPage (UIPage::SynthSettingsPage1);
  display_detail ("Loaded voice", f.name ());
}


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

const char* EFFECTS[EFFECTS_COUNT] = {"LPFC", "LPFR", "LPFA", "OMix", "O1Fr", "Freq", "O1Ph", "Phse", "LFOF", "LFOA"};
Setting settings[SETTINGS_COUNT] = {
  {"Attack (ms)", "A", 0, 100, 32000, 5, NULL},
  {"Decay (ms)", "D", 0, 100, 32000, 5, NULL},
  {"Sustain (ms) ", "S", 0, 2000, 32000, 5, NULL},
  {"Release (ms)", "R", 0, 100, 32000, 5, NULL},
  {"Osc1 Waveform", "W", 0, 1, NUM_WAVEFORMS-1, 100, TABLE_NAMES},
  {"Osc Mix", "X", 0, 100, 255, 10, NULL},
  {"Osc2 Waveform", "W", 0, 1, NUM_WAVEFORMS-1, 100, TABLE_NAMES},
  {"Osc2 Tune halfst.", "T", -40, 0, 40, 100, NULL},
  {"LFO Waveform", "W", 0, 1, NUM_WAVEFORMS-1, 100, TABLE_NAMES},
  {"LF0 Freq * 200", "F", 1, 200, 32000, 5, NULL},
  {"LF0 Amp", "A", -255, 0, 255, 10, NULL},
  {"LF0 Effect", "X", 0, 0, EFFECTS_COUNT-1, 100, EFFECTS },
  {"LPF Cutoff Freq", "C", 0, 100, 255, 10, NULL},
  {"LPF Resonance", "R", 0, 50, 170, 5, NULL},
  {"LPF Amp", "A", 0, 0, 255, 10, NULL},
  {"Nothing", "-", 0, 0, 255, 10, NULL},

  {"Effect Attack", "A", 0, 1000, 32000, 5, NULL},
  {"Effect Decay", "D", 0, 500, 32000, 5, NULL},
  {"Effect Sustain", "S", 0, 0, 32000, 5, NULL},
  {"Effect Release", "R", 0, 500, 32000, 5, NULL},
  {"Effect Amp1", "A", -255, 0, 255, 10, NULL},
  {"Effect Target1", "X", 0, 0, EFFECTS_COUNT-1, 100, EFFECTS },
  {"Effect Amp2", "A", -255, 0, 255, 10, NULL},
  {"Effect Target2", "X", 0, 0, EFFECTS_COUNT-1, 100, EFFECTS },

  {"Nothing", "-", 0, 0, 255, 10, NULL},
  {"Nothing", "-", 0, 0, 255, 10, NULL},
  {"Nothing", "-", 0, 0, 255, 10, NULL},
  {"Nothing", "-", 0, 0, 255, 10, NULL},
  {"Nothing", "-", 0, 0, 255, 10, NULL},
  {"Nothing", "-", 0, 0, 255, 10, NULL},
  {"Nothing", "-", 0, 0, 255, 10, NULL},
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
  UIPage::setCurrentPage (UIPage::SynthSettingsPage1);
  display_detail ("Saved voice", getFileName (f));
  f.close ();
}

void loadVoice (const char* name) {
  File f = openVoiceFile (name);
  f.seek (0);
  byte buf[SETTINGS_COUNT * 2];
  int len = f.read (buf, SETTINGS_COUNT * 2) / 2;
  for (int i = 0; i < len; ++i) {
    settings[i].value = (buf[i*2] << 8) | buf[i*2+1];
  }
  for (int i = len; i < SETTINGS_COUNT; ++i) {
    settings[i].value = max (0, settings[i].min); // TODO: No: initial
  }
  UIPage::setCurrentPage (UIPage::SynthSettingsPage1);
  display_detail ("Loaded voice", getFileName (f));
  f.close ();
}


#ifndef SYNTHSETTINGS_H
#define SYNTHSETTINGS_H

#include "wavetables.h"

struct Setting {
  const char* name;
  const char* shortname;
  int16_t min, value, max;
  const int16_t dynamic_res;
  const char** value_labels;
  const char* displayValue();
};

// The point of this enum is to provide readable names for the various inputs.
// For the mapping of analog pins to parameter, see the function readSettings(), further below.
enum Settings {
  AttackSetting,
  DecaySetting,
  SustainSetting,
  ReleaseSetting,

  Osc1WaveFormSetting,
  OscMixSetting,
  Osc2WaveFormSetting,
  Osc2FreqSetting,

  LFOWaveFormSetting,
  LFOFreqSetting,
  LFOAmpSetting,
  LFOEffectSetting,

  LPFCutoffSetting,
  LPFResonanceSetting,
  LPFAmpSetting,
  NothingSetting,

  SETTINGS_COUNT
};
extern Setting settings[SETTINGS_COUNT];

void saveVoice ();
void loadVoice ();

#endif

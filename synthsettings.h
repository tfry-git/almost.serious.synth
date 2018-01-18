#ifndef SYNTHSETTINGS_H
#define SYNTHSETTINGS_H

#include "wavetables.h"

// TOOD: Keeping both const and non-const vlues inside the same struct is sort of wasteful on RAM... Migh want to change that, eventually.
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

// page 2
  EffectAttackSetting,
  EffectDecaySetting,
  EffectSustainSetting,
  EffectReleaseSetting,

  EffectAmp1Setting,
  EffectWhat1Setting,
  EffectAmp2Setting,
  EffectWhat2Setting,

  NothingSetting1,  // always need 16 settings per page, in currently implementation
  NothingSetting2,
  NothingSetting3,
  NothingSetting4,

  NothingSetting5,
  NothingSetting6,
  NothingSetting7,
  NothingSetting8,

  SETTINGS_COUNT
};
extern Setting settings[SETTINGS_COUNT];

enum Effects {
  LowPassFilterCutOffEffect,
  LowPassFilterResonanceEffect,
  LowPassFilterAmpEffect,
  OscMixEffect,
  Osc1FreqEffect,
  BothOscFreqEffect,
  Osc1PhaseEffect,
  BothOscPhaseEffect,
  LFOFreqEffect,
  LFOAmpEffect,
  EFFECTS_COUNT
};

void saveVoice (const char* name);
void loadVoice (const char* name = "");

#endif

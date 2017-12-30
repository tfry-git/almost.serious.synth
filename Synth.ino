/*  Almost Serious Synth, with - currently - 15 adjustable parameters, and polyphony.
 *
 *  See README.md for details and instructions.
 *  
 *
 *  Copyright (c) 2017 Thomas Friedrichsmeier
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

#if defined(__AVR__)
#error Sorry: It would not be too hard to get this sketch to compile for an 8bit processor, but it's way too complex for a tiny AVR. Written for and tested on an STM32F103C8T6, so no expensive hardware required.
#endif

#include "display.h"
#include "encoder.h"
#include "matrix.h"
#include "storage.h"

#include <MIDI.h>
#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_midi.h>
#include <mozzi_rand.h>
#include <ADSR.h>
#include <LowPassFilter.h>
#include <WaveShaper.h>

#include "wavetables.h"

// number of polyphonic notes to handle at most. Increasing this carries the risk of overloading the processor
#define NOTECOUNT 12

// Rate (Hz) of calling updateControl(), powers of 2 please.
#define CONTROL_RATE 128
// Actually, we're calling control rate four times as often, but omitting most expensive stuff.
#define CONTROL_RATE_MULT 10
uint8_t control_loop_num;

char printf_buf[8]; // Hackish buffer for use in Setting::displayValue()

struct Setting {
  const char* name;
  const char* shortname;
  int16_t min, value, max;
  int16_t dynamic_res;
  const char** value_labels;
  const char* displayValue() {
    if (value_labels) {
      return value_labels[value];
    }
    cheap_itoa (printf_buf, value, 5);
    return printf_buf;
  }
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
uint8_t current_setting = NothingSetting;

class Note {
public:
  byte note; // MIDI note value
  int8_t velocity;
  ADSR<CONTROL_RATE, CONTROL_RATE> env;
  int8_t current_vol; // (envelope * MIDI velocity)
  bool isPlaying () { return env.playing (); };

  FlexOscil<AUDIO_RATE> oscil;
  FlexOscil<AUDIO_RATE> oscil2;
  uint8_t osc2_mag; // volume of Oscil2 relative to Oscil1, given from 0 (only Oscil1) to 255 (only Oscil2)
  uint8_t osc2_mag_base;
  Q16n16 osc1_f_base;  // NOTE: We don't shy away from float for performance reasons, but using float (and esp. mtof(float) pulls in a lot of flash-hogging floating point emulation code. Avoiding this one, alone, saves more than 7k in flash!)
  Q16n16 osc2_f_base;  //       Further avoiding all other uses of float in favor of Q16n16 and Q24n8 saves another 600 bytes or so. (YMMV depending on floating point use in graphics libs, etc.)

  // NOTE: Here, I'm using separate LFOs for each note, but there's also a point to be made for keeping all LFOs in sync (i.e. a single global lfo).
  // The LFO will be used to modulate one of a number of parameters
  FlexOscil<CONTROL_RATE> lfo;
  uint8_t lfo_amp;
  LowPassFilter lpf;
  uint8_t lpf_cutoff_base;
  uint8_t lpf_resonance_base;
  uint8_t lpf_amp_base, lpf_amp;
};
Note notes[NOTECOUNT];

void saveVoice () {
  File f = defaultVoiceSaveHandle ();
  f.seek (0);
  int16_t vals[SETTINGS_COUNT];
  for (int i = 0; i < SETTINGS_COUNT; ++i) {
    int16_t val = settings[i].value;
    f.write ((byte) (val >> 8));
    f.write ((byte) (val & 0xFF));
  }
  f.close ();
}

void loadVoice () {
  File f = defaultVoiceReadHandle ();
  byte buf[SETTINGS_COUNT * 2];
  int len = f.read (buf, SETTINGS_COUNT * 2) / 2;
  for (int i = 0; i < len; ++i) {
    settings[i].value = (buf[i*2] << 8) | buf[i*2+1];
  }
  f.close ();
}

#include <EventDelay.h>
EventDelay noteDelay;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void setup() {
  setup_display ();
  display_detail ("Starting:", "Storage");
  setup_storage ();

  display_detail ("Starting:", "MIDI");
  MIDI.begin();
  MIDI.setHandleNoteOn(MyHandleNoteOn);
  MIDI.setHandleNoteOff(MyHandleNoteOff);
  for (byte i = 0; i < NOTECOUNT; ++i) {
    notes[i].env.setADLevels(200,100);
    notes[i].env.setDecayTime(100);
    notes[i].env.setSustainTime(1000);
    notes[i].note = 0;
  }
  current_setting = 0;

  noteDelay.set (200);
  noteDelay.start ();

//  randSeed(); -- hangs?!
  display_detail ("Starting:", "Mozzi");
  startMozzi(CONTROL_RATE);
  setup_encoder ();
  keypad.init(keyrows, keycols);

  loadVoice ();
  for (uint8_t i = 0; i < SETTINGS_COUNT-1; ++i) {
    drawIconForSetting (i);
  }
  display_detail ("Startup", "complete");
}

void drawIconForSetting (uint8_t setting) {
    display_icon (setting / 4, setting % 4, settings[setting].shortname, settings[setting].displayValue());
}

void readSettings() {
  bool update = false;

  int key = keypad.read ();
  if (key >= 0) {
    if (key == NothingSetting) {
      if (noteDelay.ready ()) {
        MyHandleNoteOn (1, rand (20) + 77, 100);
        noteDelay.start ();
        saveVoice ();
      }
    } else {
      update = update || (current_setting != key);
      current_setting = key;
    }
  }

  Setting &setting = settings[current_setting];

  int8_t res = read_encoder ();
  if (res != 0) {
    update = true;
    int mult = setting.value / setting.dynamic_res + 1;
    setting.value += res * mult;
    if (setting.value < setting.min) setting.value = setting.min;
    if (setting.value > setting.max) setting.value = setting.max;
  }

  if (update) {
    drawIconForSetting (current_setting);
    display_detail(setting.name, setting.displayValue());
  }
}

// Update parameters of the given notes. Usually either called with a single note, or all notes at once.
void updateNotes (Note *startnote, uint8_t num_notes) {
  for (uint8_t i = 0; i < num_notes; ++i) {
    Note &note = startnote[i];
    note.env.setAttackTime(settings[AttackSetting].value);
    note.env.setDecayTime(settings[DecaySetting].value);
    note.env.setSustainTime(settings[SustainSetting].value);
    note.env.setReleaseTime(settings[ReleaseSetting].value);

    note.oscil.setTableNum (settings[Osc1WaveFormSetting].value);
    note.oscil2.setTableNum (settings[Osc2WaveFormSetting].value);
    note.osc2_f_base = Q16n16_mtof ((note.note + settings[Osc2FreqSetting].value) << 16);
    note.oscil2.setFreq_Q24n8 (Q16n16_to_Q24n8 (note.osc2_f_base));
    note.osc2_mag = note.osc2_mag_base = settings[OscMixSetting].value;

    // LPF
    note.lfo_amp = settings[LFOAmpSetting].value;
    note.lfo.setTableNum (settings[LFOWaveFormSetting].value);
    if (IS_NOISE_TABLE(settings[LFOWaveFormSetting].value)) {
      // HACK: For LFO, noise waveform is best served _slow_
      note.lfo.setFreq_Q16n16 (((Q16n16) settings[LFOFreqSetting].value << 16) / 10000);
    } else {
      note.lfo.setFreq_Q24n8 (((Q24n8) settings[LFOFreqSetting].value << 8) / 200);
    }
    note.lpf_resonance_base = settings[LPFResonanceSetting].value;
    note.lpf.setResonance (note.lpf_resonance_base);
    note.lpf_cutoff_base = settings[LPFCutoffSetting].value;
    note.lpf.setCutoffFreq (note.lpf_cutoff_base);
    note.lpf_amp = note.lpf_amp_base = settings[LPFAmpSetting].value;
  }
}

uint8_t u8ranged (int input) {
  if (input < 0) return 0;
  if (input > 255) return 255;
  return input;
}

void updateControl(){
  MIDI.read();

  readSettings ();

  // If you enable the line below, here (and disable the corresponding line in MyHandleNoteOn(), notes _already playing_ will be affected by pot settings.
  // Of course, updating more often means more more CPU load. You may have to reduce the NOTECOUNT.
  // updateNotes (notes, NOTECOUNT);

  for (byte i = 0; i < NOTECOUNT; ++i) {
    Note &note = notes[i];
    if (!note.isPlaying()) continue;
    note.env.update ();
    note.current_vol = note.env.next () * note.velocity >> 8;

    int16_t lfo_effect = note.lfo.next()*note.lfo_amp;
    if (settings[LFOEffectSetting].value == 0) {
      note.lpf.setCutoffFreq(u8ranged (note.lpf_cutoff_base+(lfo_effect >> 8)));
    } else if (settings[LFOEffectSetting].value == 1) {
      note.lpf.setResonance(u8ranged (note.lpf_resonance_base+(lfo_effect >> 9)));
    } else if (settings[LFOEffectSetting].value == 2) {
      note.lpf_amp = u8ranged (note.lpf_amp_base + (lfo_effect >> 8));
    } else if (settings[LFOEffectSetting].value == 3) {
      note.osc2_mag = u8ranged (note.osc2_mag_base + (lfo_effect >> 8));
    } else if (settings[LFOEffectSetting].value == 4) {  // NOTE: The two frequency shifting effects are pretty CPU intensive. You probably won't get full polphony while using these.
      // sorry about all the shifting (in order to avoid floats _and_ int overflow). Read it as: (note.osc1_f_base * 2 / (2 + lfo_effect)), with lfo-effect ranging from -1 to 1
      note.oscil.setFreq_Q24n8 ((Q16n16_to_Q24n8 (note.osc1_f_base) * ((1<<24) / ((1<<16) + lfo_effect))) >> 8); // corresponds to a max freq shift between .5 and 1.5 of the center
      // TODO: can we have something approaching a log scale (i.e. +/- 1 octave), without really expensive floating point stuff?
      // Perhaps going via mtof_Q16n16()? (That uses linear interpolation between notes, though, and we'll be sweeping across such ranges)
    } else if (settings[LFOEffectSetting].value == 5) {
      note.oscil.setFreq_Q24n8 ((Q16n16_to_Q24n8 (note.osc1_f_base) * ((1<<24) / ((1<<16) + lfo_effect))) >> 8);
      note.oscil2.setFreq_Q24n8 ((Q16n16_to_Q24n8 (note.osc2_f_base) * ((1<<24) / ((1<<16) + lfo_effect))) >> 8);
    }
  }
}

int updateAudio(){
  if (!(++control_loop_num % CONTROL_RATE_MULT)) update_encoder ();

  int ret = 0;
  for (byte i = 0; i < NOTECOUNT; ++i) {
    Note &note = notes[i];
    // Step 1: Mix waveforms
    int8_t unfiltered = ((int32_t) note.oscil2.next() * note.osc2_mag + (int32_t) note.oscil.next() * (255u - note.osc2_mag)) >> 8; // LPF filter only takes 8 bits as input
    // Step 2: Apply lowpass
    int32_t filtered = (int32_t) note.lpf.next(unfiltered) * note.lpf_amp + (255u-note.lpf_amp) * unfiltered;
    // Step 3: Apply envelope and add to output
    ret += (int32_t) note.current_vol * filtered >> 14;
  }
  return ret;
}

void loop(){
  audioHook(); // required here
}

void MyHandleNoteOn(byte channel, byte pitch, byte velocity) {
  if (velocity > 0) {
    for (byte i = 0; i < NOTECOUNT; ++i) {
      if (!notes[i].isPlaying ()) {
        Note &note = notes[i];
        note.note = pitch;

        // Initialize current note with current parameters. Depending on your taste and usecase, you may want to disable this, and enable the corresponding line
        // inside updateControl(), instead.
        updateNotes(&note, 1);

        note.osc1_f_base = Q16n16_mtof (pitch << 16);
        note.oscil.setPhase (0); // Make sure oscil1 and oscil2 start in sync
        note.oscil.setFreq_Q24n8 (Q16n16_to_Q24n8 (note.osc1_f_base));
        note.env.noteOn();
        note.velocity = velocity;

        // LPF
        note.lfo.setPhase (note.lfo.tableSize () / 4); // 90 degree into table; since the LFO is oscillating _slow_, we cannot afford a random starting point */

        // Wave mixing
        note.oscil2.setPhase (0);
        break;
      }
    }
  } else {
    MyHandleNoteOff (channel, pitch, velocity);
  }
}

void MyHandleNoteOff(byte channel, byte pitch, byte velocity) {
  for (byte i = 0; i < NOTECOUNT; ++i) {
    if (notes[i].note == pitch) {
      if (!notes[i].isPlaying ()) continue;
      notes[i].env.noteOff ();
      notes[i].note = 0;
      //break; Continue the search. We might actually have two instances of the same note playing/decaying at the same time.
    }
  }
}



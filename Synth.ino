/*  Almost Serious Synth, with 20+ adjustable parameters, and polyphony.
 *
 *  See README.md for details and instructions.
 *  
 *
 *  Copyright (c) 2017-2018 Thomas Friedrichsmeier
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
#error Sorry: It would not be too hard to get this sketch to compile for an 8bit processor, but it is way too complex for a tiny AVR. Written for and tested on an STM32F103C8T6, so no expensive hardware required.
#endif
#define DEFINE_NOW
// Rate (Hz) of calling updateControl(), powers of 2 please.
#define CONTROL_RATE 128

#include <MozziGuts.h>
#include <mozzi_midi.h>
#include <mozzi_rand.h>
#include <ADSR.h>
#include <LowPassFilter.h>
#include <EventDelay.h>

void saveVoice ();

#include "util.h"
#include "display.h"
#include "userinput.h"
#include "storage.h"
#include "wavetables.h"
#include "synthsettings.h"
#include "midiplayer.h"
MIDIPlayer player;
#include "ui.h"

// number of polyphonic notes to handle at most. Increasing this carries the risk of overloading the processor
// note slots are also one of the main consumers of RAM.
#define NOTECOUNT 12

// Rate (Hz) of calling updateControl(), powers of 2 please.
#define CONTROL_RATE 128

struct Note {
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
  int16_t lfo_amp_base;
  Q16n16 lfo_f_base;
  LowPassFilter lpf;
  uint8_t lpf_cutoff_base;  // NOTE: If RAM gets really tight, we could remove some of those "base" caches, and utilitize the corresponding current setting value, instead.
  uint8_t lpf_resonance_base;
  uint8_t lpf_amp_base, lpf_amp;

  ADSR<CONTROL_RATE, CONTROL_RATE> effect_env;
};
Note notes[NOTECOUNT];

//#define DO_PROFILE  // Do timings of updateAudio() and updateControL(), without actually running anything
#ifdef DO_PROFILE
int16_t asyncAnalogRead (uint8_t pin) {
  return analogRead (pin);  // a convenient lie for profiling, assuming analogRead does not make up a huge amount of the timing
}
#include <CircularBuffer.h>
#include "storage.h"
volatile int16_t do_profile_dummy; // Avoid compiler optimizations
void do_profile () {
  CircularBuffer<int> buffer;
  uint32_t oldt = millis ();
  for (uint32_t i = 0; i < AUDIO_RATE << 2; ++i) {
    do_profile_dummy = updateAudio ();
    buffer.write (do_profile_dummy);
    do_profile_dummy = buffer.read ();
  }
  uint32_t elapsed = millis () - oldt;
  char bufc[12];
  cheap_itoa (bufc, elapsed >> 2, 8);
  display_detail ("1 Audio sek:", bufc);  // number of milliseconds needed for 1 second worth of the audio loop
  delay (1000);

  oldt = millis ();
  for (uint32_t i = 0; i < CONTROL_RATE << 2; ++i) {
    do_profile_dummy++;
    updateControl ();
  }
  elapsed = millis () - oldt;
  cheap_itoa (bufc, elapsed >> 2, 8);
  display_detail ("1 Control sek:", bufc);  // number of milliseconds needed for 1 second worth of the control loop
  delay (1000);

  oldt = millis ();
  for (uint32_t i = 0; i < 10000ul; ++i) {
    do_profile_dummy++;
    MyHandleNoteOn (0, (i+NOTECOUNT) % 50 + 40, 100);
    MyHandleNoteOff (0, i % 50 + 40, 100);
  }
  elapsed = millis () - oldt;
  cheap_itoa (bufc, elapsed, 8);
  display_detail ("10000 notes:", bufc);  // number of milliseconds needed to handle 1000 note on and 1000 note off events
  delay (1000);

  File dir = openMidiDirectory ();
  File f;
  while (dir && (f = dir.openNextFile ())) {
    if (f.size () >= 5000) break;
  }
  if (dir) dir.close ();
  if (f && f.size () > 5000) {
    oldt = millis ();
    for (uint32_t i = 0; i < 10000ul; ++i) {
      if (!i % 5000) {
        f.seek (0);
      }
      do_profile_dummy += f.read ();
    }
    elapsed = millis () - oldt;
    cheap_itoa (bufc, elapsed, 8);
    display_detail ("10000 con.reads:", bufc);  // number of milliseconds needed for 10000 mostly consequtive SD reads
    delay (1000);

    oldt = millis ();
    for (uint32_t i = 0; i < 10000ul; ++i) {
      f.seek ((i * 967 % 5000));
      do_profile_dummy += f.read ();
    }
    elapsed = millis () - oldt;
    cheap_itoa (bufc, elapsed, 8);
    display_detail ("10000 rnd.reads:", bufc);  // number of milliseconds needed for 10000 mostly "random" SD reads
    delay (1000);
  }
}
#else
int16_t asyncAnalogRead (uint8_t pin) {
  return mozziAnalogRead (pin);
}
#endif

void setup() {
  pinMode (LED_BUILTIN, OUTPUT);
  setup_display ();
  display_detail ("Starting:", "Storage");
  setup_storage ();

  display_detail ("Starting:", "MIDI");
  player.setupMIDI ();
  for (byte i = 0; i < NOTECOUNT; ++i) {
    notes[i].env.setADLevels(200,100);
    notes[i].effect_env.setSustainTime(64000);
    notes[i].note = 0;
  }

  //setup_keypad ();
  setup_updown ();

  display_detail ("Loading:", "Settings");
  loadVoice ();
  UIPage::setCurrentPage(UIPage::SynthSettingsPage1);
  player.playRandom ();

#ifdef DO_PROFILE
  do_profile ();
#else
  display_detail ("Starting:", "Mozzi");
  startMozzi(CONTROL_RATE, NO_SETUP_MOZZI_ADC);
  display_detail ("Startup", "complete");
#endif
}

// Update parameters of the given notes. Usually either called with a single note, or all notes at once.
void updateNotes (class Note *startnote, uint8_t num_notes) {
  for (uint8_t i = 0; i < num_notes; ++i) {
    Note &note = startnote[i];
    note.env.setAttackTime(settings[AttackSetting].value);
    note.env.setDecayTime(settings[DecaySetting].value);
    note.env.setSustainTime(settings[SustainSetting].value);
    note.env.setReleaseTime(settings[ReleaseSetting].value);

    note.effect_env.setAttackTime(settings[EffectAttackSetting].value);
    note.effect_env.setDecayTime(settings[EffectDecaySetting].value);
    note.effect_env.setADLevels(settings[EffectAttackLevelSetting].value, settings[EffectDecayLevelSetting].value);

    note.oscil.setTableNum (settings[Osc1WaveFormSetting].value);
    note.oscil2.setTableNum (settings[Osc2WaveFormSetting].value);
    note.osc2_f_base = Q16n16_mtof ((note.note + settings[Osc2FreqSetting].value) << 16);
    note.oscil2.setFreq_Q24n8 (Q16n16_to_Q24n8 (note.osc2_f_base));
    note.osc2_mag = note.osc2_mag_base = settings[OscMixSetting].value;

    // LPF
    note.lfo_amp_base = settings[LFOAmpSetting].value;
    note.lfo.setTableNum (settings[LFOWaveFormSetting].value);
    note.lfo_f_base = ((Q16n16) settings[LFOFreqSetting].value << 16) / 200;
    if (IS_NOISE_TABLE(settings[LFOWaveFormSetting].value)) {
      note.lfo.setFreq_Q16n16 (note.lfo_f_base >> 8);
    } else {
      note.lfo.setFreq_Q24n8 (Q16n16_to_Q24n8 (note.lfo_f_base));
    }
    note.lpf_resonance_base = settings[LPFResonanceSetting].value;
    note.lpf.setResonance (note.lpf_resonance_base);
    note.lpf_cutoff_base = settings[LPFCutoffSetting].value;
    note.lpf.setCutoffFreq (note.lpf_cutoff_base);
    note.lpf_amp = note.lpf_amp_base = settings[LPFAmpSetting].value;
  }
}

void updateControl() {
//  uint8_t buf1 = bufferedSamples ();
  player.update ();
//  digitalWrite (PC13, buf1 - bufferedSamples () < 100);

  UIPage::currentPage ()->handleButton (read_keypad ());
  UIPage::currentPage ()->handleUpDown (read_updown ());
  UIPage::currentPage ()->handleLeftRight (read_leftright ());
  UIPage::currentPage ()->updateDisplay ();

  // If you enable the line below, here (and disable the corresponding line in MyHandleNoteOn(), notes _already playing_ will be affected by pot settings.
  // Of course, updating more often means more CPU load. You may have to reduce the NOTECOUNT.
  // updateNotes (notes, NOTECOUNT);

  for (byte i = 0; i < NOTECOUNT; ++i) {
    Note &note = notes[i];
#if not defined (DO_PROFILE)
    if (!note.isPlaying()) continue;
#endif
    note.env.update ();
    note.effect_env.update ();
    note.current_vol = note.env.next () * note.velocity >> 8;

    uint8_t effectenv_cur = note.effect_env.next ();
    int16_t effects[EFFECTS_COUNT] = {0};
    effects[settings[EffectWhat1Setting].value] = (int32_t) effectenv_cur * settings[EffectAmp1Setting].value / 2;
    effects[settings[EffectWhat2Setting].value] += effectenv_cur * settings[EffectAmp2Setting].value / 2;
    effects[settings[LFOEffectSetting].value] += (int32_t) note.lfo.next()*(note.lfo_amp_base + (effects[LFOAmpEffect] / 256));

    for (uint8_t i = 0; i < EFFECTS_COUNT; ++i) {
      // TODO: Applying all effects in each iteration is sort of wasteful, but profiling suggests that optimizations, here, are not a high priority.
      int16_t effect_size = effects[i];

      if (i == LowPassFilterCutOffEffect) {
        note.lpf.setCutoffFreq(u8ranged (note.lpf_cutoff_base+(effect_size / 256)));  // note: division not bitshift for defined behavior on signed int
      } else if (i == LowPassFilterResonanceEffect) {
        note.lpf.setResonance(u8ranged (note.lpf_resonance_base+(effect_size / 512)));
      } else if (i == LowPassFilterAmpEffect) {
        note.lpf_amp = u8ranged (note.lpf_amp_base + (effect_size / 256));
      } else if (i == OscMixEffect) {
        note.osc2_mag = u8ranged (note.osc2_mag_base + (effect_size / 256));
      } else if (i == Osc1FreqEffect) {
        // sorry about all the shifting (in order to avoid floats _and_ int overflow). Read it as: (note.osc1_f_base * 2 / (2 + effect_size)), with effect_size ranging from -1 to 1
        note.oscil.setFreq_Q24n8 ((Q16n16_to_Q24n8 (note.osc1_f_base) * ((1<<24) / ((1<<16) + effect_size))) >> 8); // corresponds to a max freq shift between .5 and 1.5 of the center
        // TODO: can we have something approaching a log scale (i.e. +/- 1 octave), without really expensive floating point stuff?
        // Perhaps going via mtof_Q16n16()? (That uses linear interpolation between notes, though, and we'll be sweeping across such ranges)
      } else if (i == BothOscFreqEffect) {
        note.oscil.setFreq_Q24n8 ((Q16n16_to_Q24n8 (note.osc1_f_base) * ((1<<24) / ((1<<16) + effect_size))) >> 8);
        note.oscil2.setFreq_Q24n8 ((Q16n16_to_Q24n8 (note.osc2_f_base) * ((1<<24) / ((1<<16) + effect_size))) >> 8);
      } else if (i == Osc1PhaseEffect) {
        note.oscil.setPhaseModulation((Q15n16) effect_size);
      } else if (i == BothOscPhaseEffect) {
        note.oscil.setPhaseModulation((Q15n16) effect_size);
        note.oscil2.setPhaseModulation((Q15n16) effect_size);
      } else if (i == LFOFreqEffect) {
        if (IS_NOISE_TABLE(settings[LFOWaveFormSetting].value)) {
          // HACK: For LFO, noise waveform is best served _slow_
          note.lfo.setFreq_Q16n16 ((Q16n16_to_Q24n8 (note.lfo_f_base) * ((1<<24) / ((1<<16) + effect_size*2))) >> 16);  // NOTE: modulation effect intentionally larger for LFO than for main Oscillators
        } else {
          note.lfo.setFreq_Q24n8 ((Q16n16_to_Q24n8 (note.lfo_f_base) * ((1<<24) / ((1<<16) + effect_size*2))) >> 8);
        }
      } else {
        // LFO Amp modulation already handled outside the loop
      }
    }
  }
}

int updateAudio(){
  int ret = 0;
  for (byte i = 0; i < NOTECOUNT; ++i) {
    Note &note = notes[i];
#if not defined (DO_PROFILE)
    if (!note.isPlaying ()) continue;
#endif
    // Step 1: Mix waveforms
    int8_t unfiltered = ((int32_t) note.oscil2.next() * note.osc2_mag + (int32_t) note.oscil.next() * (255u - note.osc2_mag)) >> 8; // LPF filter only takes 8 bits as input
    // Step 2: Apply lowpass
    int32_t filtered;
    if (note.lpf_amp) {
      filtered = (int32_t) note.lpf.next(unfiltered) * note.lpf_amp + (255u-note.lpf_amp) * unfiltered;
    } else {
      filtered = unfiltered << 8;
    }
    // Step 3: Apply envelope and add to output
    ret += (int32_t) note.current_vol * filtered >> 14;
  }
  return ret;
}

void loop(){
  audioHook(); // required here
#if defined (HIGH_FREQUENCY_HOOK)
  HIGH_FREQUENCY_HOOK();
#endif
}

void MyHandleNoteOn(byte channel, byte pitch, byte velocity) {
  if (velocity > 0) {
    // Find a suitable slot for the new note. If it is the same pitch as an existing note, use that, otherwise pick a fresh slot.
    byte candidate = NOTECOUNT;
    for (byte i = 0; i < NOTECOUNT; ++i) {
      Note &note = notes[i];
      if (note.note == pitch) { // found same note already playing on the same pitch: Use that
        candidate = i;
        break;
      }
      if (!note.isPlaying ()) {
        candidate = i;
      }
    }
    if (candidate >= NOTECOUNT) return;

    Note &note = notes[candidate];
    if (note.note == pitch || !note.isPlaying ()) {
      note.note = pitch;

      // Initialize current note with current parameters. Depending on your taste and usecase, you may want to disable this, and enable the corresponding line
      // inside updateControl(), instead.
      updateNotes(&note, 1);

      note.osc1_f_base = Q16n16_mtof (pitch << 16);
      note.oscil.setPhase (0); // Make sure oscil1 and oscil2 start in sync
      note.oscil.setFreq_Q24n8 (Q16n16_to_Q24n8 (note.osc1_f_base));
      note.env.noteOn();
      note.effect_env.noteOn();  // Compilation error on this line? Remove the "true", until Mozzi has merged https://github.com/sensorium/Mozzi/pull/40 . Effect envelope will behave strangley, then, however.
      note.velocity = velocity;

      // LPF
      note.lfo.setPhase (note.lfo.tableSize () / 4); // 90 degree into table; since the LFO is oscillating _slow_, we cannot afford a random starting point */

      // Wave mixing
      note.oscil2.setPhase (0);
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
      notes[i].effect_env.noteOff ();
      notes[i].note = 0;
      break;
    }
  }
}



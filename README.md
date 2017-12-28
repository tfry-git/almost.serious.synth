# Almost Serious Synth, with - currently - 15 adjustable parameters, and polyphony.

The setup is mostly inspired by AMSynth (but adds some more complex wave forms). Based on the Mozzi sound synthesis library.

## Hardware
- Written for and tested on an STM32F103C8T6 "blue pill". It should be easy to port to almost any other processor / board,
  as long as that is supported by Mozzi. However, do note that it is simply too much for an 8bit processor,
  so not Arduino Uno and friends.
-- *Note* that this project assumes 128kB available flash (around 70k of that is used at the time of this writing). While the STM32F103C8T6 is
   officially spec'ed at 64kB flash, (almost) all of these MCUs seem to come with 128kB, actually. _However_, if yours does not, or you
   do not want to stretch your luck, simply remove some of the wave-tables, or use a lower sampling resolution for the tables to import (for both,
   just edit wavetables.h).
- Uses a 4x4 keypad matrix to select one of the settings (bottom right button to play a random note),
  pin connections defined in matrix.h .
- Uses a rotary encoder to adjust the current setting. Connections defined in encoder.h . You can replace
  this with simple +/- buttons or a pot - all you will have to do is provide appropriate versions of update_encoder()
  and read_encoder() (trivial for buttons).
- Uses a 128*64 pixel SSD1306 display with I2C interface, on the default I2C pins (PB6, PB7). To use a different display,
  edit display.h to your liking. However you'll have a hard time making do with any lower resolution.
- MIDI connected on Serial1, i.e. PA9, PA10. So far, only RX is used.
- Audio output on pin PB8 - you can connect a headphone, directly - see Mozzi documentation.

## Synthesizer settings:
- Top row: Envelope - Attack, Decay, Sustain, Release
- Row 2: Wave mixing - Oscillator 1 waveform, Oscillator mix ratio, Oscillator 2 waveform, Oscillator 2 tune, given in half-tones above below Oscillator 1
- Row 3: Low frequency oscillator: Waveform, Frequency, Amplitude (initially set to 0, i.e. disabled), Parameter to modulate
- Bottom 4: Low pass filter: Cutoff frequency, Resonance, Amplitude (initially set to 0, i.e. unfiltered)
- Bottom - right button: Play a random note (very useful for testing during development).

## Libraries
- Requires Mozzi with STM32 support. For the time being, get it from here: https://github.com/tfry-git/Mozzi  (until available in the official Mozzi)
- Requires the Maple-based STM32 core at https://github.com/rogerclarkmelbourne/Arduino_STM32 *in a very recent version*

## Future directions
I plan to add an SD slot along with the ability to load and save sounds, and to record and play back MIDI sequences.
There's also some flash and CPU power left to add more effects!

You're very much invited to contribute! Or as another way to support this project, consider donating via paypal to thomas.friedrichsmeier@gmx.de .
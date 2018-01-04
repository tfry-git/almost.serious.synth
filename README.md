# Almost Serious Synth, with - currently - 15 adjustable parameters, MIDI recording, and polyphony.

The setup is mostly inspired by AMSynth (but adds some more complex wave forms). Based on the Mozzi sound synthesis library.

## Hardware
- Written for and tested on an STM32F103C8T6 "blue pill". It should be easy to port to almost any other processor / board,
  as long as that is supported by Mozzi. However, do note that it is simply too much for an 8bit processor,
  so not Arduino Uno and friends.
  - *Note* that this project assumes 128kB available flash (around 70k of that is used at the time of this writing). While the STM32F103C8T6 is
   officially spec'ed at 64kB flash, (almost) all of these MCUs seem to come with 128kB, actually. _However_, if yours does not, or you
   do not want to stretch your luck, simply remove some of the wave-tables, or use a lower sampling resolution for the tables to import (for both,
   just edit wavetables.h).
- Uses a 4x4 keypad matrix to select one of the settings (bottom right button to exit to "menu"),
  pin connections defined in matrix.h (default is PA4-7 for the rows, PB0, PB1, PB10, PB11 for the cols).
- Uses a single potentiometer (or joystick) to adjust the current setting up / down (wiper connected to PA0).
  - _Alternatively_, you can also use a rotary encoder, by adjusting just a few defines in userinput.h.
  - You could replace also this with simple +/- buttons - all you will have to do is provide appropriate versions of setup_updown() and read_updown()
  (trivial for buttons).
- Uses a 128*64 pixel SSD1306 display with I2C interface, on the default I2C pins (PB6, PB7). To use a different display,
  edit display.cpp to your liking. However you'll have a hard time making do with any lower resolution.
- MIDI connected on Serial1, i.e. PA9, PA10. So far, only RX is used.
- Audio output on pin PB8 - you can connect a headphone, directly - see Mozzi documentation.
  - Optionally an audio-amp and speaker connected to PB8.
- An SD card reader is connected on SPI2 (pins PB12(CS) through PB15). Optional.

## Libraries
- Requires Mozzi with STM32 support. For the time being, get it from here: https://github.com/tfry-git/Mozzi  (until available in the official Mozzi)
  - It is recommendable to up the AUDIO_RATE to 32768 in mozzi_config.h
- Requires the Maple-based STM32 core at https://github.com/rogerclarkmelbourne/Arduino_STM32 *in a very recent version*

## Usage / UI
At the time of this writing, the UI consist of two pages, one for the Synthesizer settings, one for the "main menu". Both are laid out in a logical
4 by 4 matrix, corresponding to the buttons of the keypad. So, to select the setting / activate the option in row 2, colum 3, you press the button in
row 2, column 3, etc.

For the time being, when starting, the Synth will play the latest MIDI recording (if any, random notes, otherwise), which is pretty useful during development. To turn this off,
go the main menu, and select "Stop" (row 3, column 2).

## Synthesizer settings
- Top row: Envelope - Attack, Decay, Sustain, Release
- Row 2: Wave mixing - Oscillator 1 waveform, Oscillator mix ratio, Oscillator 2 waveform, Oscillator 2 tune, given in half-tones above below Oscillator 1
- Row 3: Low frequency oscillator: Waveform, Frequency, Amplitude (initially set to 0, i.e. disabled), Parameter to modulate
- Bottom 4: Low pass filter: Cutoff frequency, Resonance, Amplitude (initially set to 0, i.e. unfiltered)
- Bottom - right button: Exit to main menu

## Main menu
- Top row: This is just a caption for Synthesizer options in  the next row
- Row 2: "Edit" -> Go to the synthesizer settings page; "Save"/"Load" -> Save / load current voice to/from SD card
- Row 3: Caption for the MIDI related options in the next row
- Row 4: "Rec/Stop" -> Start / end recording MIDI from MIDI in; "Play/Stop" -> Play / stop playing the current MIDI sequence; "Save"/"Load" -> Save / load MIDI track to/from SD card

## Waveforms
One of the things that may not be self-explanatory is probably the waveforms. First, there are the usual "basic" waveforms square ("Squ"), sine ("Sin"), saw / ramp ("Saw"),
trinagle ("Tri"), and white noise ("Noi"). In addition, two more complex waveforms (short loopable samples, really) are included, which help a lot in 
creating (un-)natural-sounding voices: "Str" (a guitar string), and "Aah" (a choir Aah).

Each waveform is also available passed through different wave-shaping functions, signified by the third letter replaced by a suffix: Chebyshev 3rd order (suffix "3"),
Chebyshev 5th order (suffix "5"), and simoid (suffix "/").

Not all combinations of waveform / wave-shaping function make a lot of sense, and not all waveforms make sense e.g. when used for the LFO. However, the current setup was
both easy to code, and (I think) easy to understand.

## Limitations and notes
- The Synth is - technically - 12 note polyphnic by default (easily adjustable via a compile time define), but when really playing 12 simultaneous notes, and in particular when playing
  many simultaneous notes via some of the more CPU intensive effects enabled (such as Frequency modulation), you may start hearing "clicks" in the audio output, corresponding to
  buffer underruns.
  - Preliminary debugging suggests the bottleneck is in updateAudio()
- While adjusting settings or navigating the menu, audio will be severely disrupted. This is due to the fact that the I2C communication used with the display is synchronous (i.e. blocking).
- This is all a work in progess. Some things _will_ break with an update, but I'll try not to break things, badly.
- Some additional circuitry may be needed to get rid of noise. Two notes here:
  - The SSD1306 display introduces a low hum, interestingly somtimes much louder, sometimes much more quiet. Disconnecting the power to the display removes the hum.
  - When connecting the audio out to an amplifier (in particular a Class D amplifier), you will want to add a simple low pass filter to strip out the PWM carrier.
- MIDI playback / recording
  - Theoretically, the Synth can play back arbitray Format 0 and Format 2 MIDI files - but that theory is not well tested.
  - Format 1 files will not be played, correctly, so far
  - The recorded MIDI is a valid format 0 file - again not much tested, though
  - NoteOn and NoteOff are the only events handled, so far, but most events are recorded.

## Future directions
For now the Synth will save a single set of synth settings on the SD card ("/voices/0.voc"), and restore them on startup. Similarly, you can record and playback exactly one
MIDI sequence ("/MIDI/0.mid"). Of course the plan is to offer to store an arbitrary number of named voices / sequences.

There's also some flash and CPU power left to add more synth effects! The limit currently is the UI / display, really. To add anything more, we'll need more
"pages" of settings, or a larger display.

It may make sense to switch to larger IL9341 based TFT. One popular offering at the time of this writing - while twice the price of an SSD1306 display - sports a 240*320 resolution,
includes an SD slot _and_ features a touch-screen, which could allow for a pretty intuitive UI. At any rate, the idea is to keep the project mostly modular, so it can be adjusted
to different hardware, easily.

You're very much invited to contribute! Or as another way to support this project, consider donating via paypal to thomas.friedrichsmeier@gmx.de .
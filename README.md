# Almost Serious Synth, with 20+ adjustable parameters, MIDI recording, and polyphony.

- Based on the Mozzi sound synthesis library.
- The Synth setup is mostly inspired by AMSynth (but adds some more complex wave forms).
- Features MIDI playback (including Format 1) from SD card (but separate voices per channel not yet implemented)
  and MIDI recording.

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

For the time being, when starting, the Synth will play random notes on startup, which is pretty useful during development. To turn this off,
go the main menu, and select "Stop" (row 3, column 2). It will load the first saved voice on startup.

## Synthesizer settings - Page 1
- Top row: Envelope - Attack, Decay, Sustain, Release
- Row 2: Wave mixing - Oscillator 1 waveform, Oscillator mix ratio, Oscillator 2 waveform, Oscillator 2 tune, given in half-tones above below Oscillator 1
- Row 3: Low frequency oscillator: Waveform, Frequency, Amplitude (initially set to 0, i.e. disabled), Parameter to modulate
- Bottom row: Low pass filter: Cutoff frequency, Resonance, Amplitude (initially set to 0, i.e. unfiltered)
- Bottom - right button: Exit to main menu

## Synthesizer settings - Page 2
- Top row: Envelope for the effects given in the next row - Attack time, Attack level, Decay time, Decay level (envelope will remain at decay level after the decay period)
- Row 2: Two effects controlled by the above envelope, each with separate max amplitude ("A"), and effect target ("X")
- Row 3: - Not yet used -
- Bottom row: - Not yet used -
- Bottom - right button: Exit to main menu

## Main menu
- Top row: This is just a caption for Synthesizer options in  the next row
- Row 2: "Edit" -> Go to the synthesizer settings page 1; "Edi2" -> Go to synthesizer settings page 2; "Save"/"Load" -> Save / load current voice to/from SD card
- Row 3: Caption for the MIDI related options in the next row
- Row 4: "Rec/Stop" -> Start / end recording MIDI from MIDI in; "Play/Stop" -> Play / stop playing the current MIDI sequence; "Save"/"Load" -> Save / load MIDI track to/from SD card

## Saving / loading voices / recordings
I hope these are pretty self-explanatory, if clumsy. Otherwise, ask. Suggestions for improvements welcome.

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
  many simultaneous notes via some of the more CPU intensive effects enabled (such as Frequency modulation), you may start hearing chopped audio or "clicks" in the audio output, corresponding to
  buffer underruns.
  - Preliminary debugging suggests that the largest consumer of CPU power is updateAudio (35-45% of CPU at 32768Hz audio rate, and twelve notes playing),
    but a large part of the problem is IO delays inside updateControl(). Use the DO_PROFILE define in the main source to get some numbers for tuning.
- While adjusting settings or navigating the menu, audio will be severely disrupted. This is due to the fact that the I2C communication used with the display is synchronous (i.e. blocking).
- This is all a work in progess. Some things _will_ break with an update, but I'll try not to break things, badly.
- Some additional circuitry may be needed to get rid of noise. Two notes here:
  - The SSD1306 display introduces a low hum, changing with the display content. The hum is reduced a lot (but does not quite go away) when powering the display from the 5V rail. Perhaps try
    equipping it with a voltage regulator of its own (I have not tried, yet)
  - When connecting the audio out to an amplifier (in particular a Class D amplifier), you will want to add a simple low pass filter to strip out the PWM carrier.
- MIDI playback / recording
  - The Snyth will play back MIDI formats 0, 1, and 2 from SD card (so far all tracks in a same voice, however)
  - Should you find complex MIDI format 1 files play - mostly - ok, but with lag or chopping, the most likely cause is lack of IO performance. You can improve this, significantly,
    by using Bill Greimans SdFat library (https://github.com/greiman/SdFat) instead of the "default" Arduion SD library. Uncomment the "#define USE_SDFAT" in storage.h to
    enable the SdFat library.
  - The recorded MIDI is a valid format 0 file - playing it elsewhere is not tested, though
  - NoteOn and NoteOff are the only events handled, so far, but most events are recorded.

## Future directions
Saving / loading of voices / MIDI files is still pretty clumsy. That needs improvements.

One obviously useful extension will be to allow multi-track MIDI recordings (as we already support playing back multi-track MIDI files, the main trouble will be the UI), to turn
the Synth into a full MIDI sequencer. To go along with that the ability to set separate voices for each track.

There's also some flash and CPU power left to add more synth effects! The limit currently is the UI / display, really. Some ideas on _what_ to add:
- Oscillator sync
- Ring mod
- More time based effects (line / phasor) - but that may actually be overkill?
- Reverb - but that needs lots of RAM
- Filters other than LPF
- Allow MIDI velocity to modify effects

It may make sense to switch to larger IL9341 based TFT. One popular offering at the time of this writing - while twice the price of an SSD1306 display - sports a 240*320 resolution,
includes an SD slot _and_ features a touch-screen, which could allow for a pretty intuitive UI. At any rate, the idea is to keep the project mostly modular, so it can be adjusted
to different hardware, easily.

You're very much invited to contribute! Or as another way to support this project, consider donating via paypal to thomas.friedrichsmeier@gmx.de .
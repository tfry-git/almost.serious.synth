#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include <midi_Defs.h>

#include "storage.h"

/** This class is meant to handle recording and playback of MIDI events. */
class MIDIPlayer {
public:
  void play () {
    stop ();
    state = Playing;
    io = defaultMIDIRecHandle ();
    io.seek (0);
    event_time = nowTime() + readLong ();
  }
  
  void playRandom () {
      stop ();
//      randSeed (mozziMicros ());  -- hangs?!
      state = PlayingRandom;
  }
  void stop () {
    io.close ();
    state = Stopped;
  }
  void record () {
    stop ();
    state = Recording;
    event_time = 0;
    io = defaultMIDIRecHandle ();
    io.seek (0);
  }
  void update () {
    while (MIDI.read ()) {
      if (state == Recording) {
        midi::MidiType mtype = MIDI.getType ();
        if (mtype != midi::InvalidType && mtype < midi::SystemExclusive) {
          writeLong (getDelta());
          io.write ((byte) mtype | MIDI.getChannel ());
          io.write (MIDI.getData1());
          if (MidiMessageLength (mtype) > 2) {  // including the command byte
            io.write (MIDI.getData2());
          }
        }
      }
    }

    if (state == Stopped) return;

    if (state == Playing) {
      if (io.available()) {
        uint32_t now = nowTime();
        while (event_time <= now) {
          byte buf[4];
          byte in;
          byte pos = 0;
          in = io.read ();  // This should be the command byte
          while (!(in & 0x80)) { // But we got something else? Fast forward to the next elegible byte, and _try_ to make some sense from there.
            // Which is not a really good strategy, either, as - contrary to a live stream - we will probably probably get screwed by a delta time, instead...
            if (!io.available ()) return;
            in = io.read ();
          }
          midi::MidiType mtype = MIDI.getTypeFromStatusByte (in);
          buf[0] = in;
          while (++pos < MidiMessageLength (mtype)) { // read all data bytes
            // NOTE: We will hopelessly screw up on events over than those that we write during record(). This is _not_ a universal MIDI player, ATM.
            buf[pos] = io.read ();
          }
          switch (mtype) {
            case midi::NoteOn:
              MyHandleNoteOn (buf[0] % 0x7F, buf[1], buf[2]);
              break;
            case midi::NoteOff:
              MyHandleNoteOff (buf[0] % 0x7F, buf[1], buf[2]);
              break;
            // default: fail, silently... TODO implement more events
          }

          if (!io.available ()) break;
          event_time += readLong ();
        }
      } else {
        playRandom ();
      }
    }
    
    if (state == PlayingRandom) {
      if (noteDelay.ready ()) {
        MyHandleNoteOn (1, rand (20) + 77, 100);
        noteDelay.start (1000);
      }
    }
  }
  bool isPlaying () const {
    return (state == Playing || state == PlayingRandom);
  }
  bool isRecording () const {
    return (state == Recording);
  }
private:
  // Write a MIDI variable length entry. Max four bytes, seven bits per byte 0x80 to signify continuation
  void writeLong (uint32_t val) {
    byte buf[4];
    int8_t pos = 0;
    buf[0] = val & 0x7F;
    while (val > 0x7F) {
      val = val >> 7;
      buf[++pos] = 0x80 + (val & 0x7F);
    }
    while (pos >= 0) {
      io.write (buf[pos]);
      --pos;
    }
  }

  uint32_t readLong () {
    byte in;
    uint32_t ret = 0;
    byte pos = 0;
    do {
      in = io.read ();
      ret = (ret << 7) + (in & 0x7F);
    } while ((in & 0x80) && (++pos < 4)); // The length check may seem overcautios on first glance, but on end of file, io.read() will return -1, i.e. always have the 0x80 bit.
    return ret;
  }
  uint32_t getDelta () {
    uint32_t ret;
    uint32_t now = nowTime();
    if (!event_time) {
      ret = 0;
    } else {
      ret = now - event_time; // overflow safe?
    }
    event_time = now;
    return ret;
  }
  // To be adjusted for ticks and such.
  uint32_t nowTime () {
      return millis ();
  }
  byte MidiMessageLength (byte mtype) const {
    // TODO: This is correct only for the MIDI events we record ourselves. not for everything else.
    if (mtype == midi::ProgramChange || mtype == midi::AfterTouchChannel) return 2;
    return 3;
  }
  enum PlayerState {
    Stopped,
    Playing,
    PlayingRandom,
    Recording
  };
  uint32_t event_time;  // timestamp of the previous (for recording) or next (for playback) event in the stream
  PlayerState state = Stopped;
  EventDelay noteDelay;
  File io;
};

#endif

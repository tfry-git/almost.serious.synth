#include "midiplayer.h"

#include <MIDI.h>
#include <midi_Defs.h>
#include <mozzi_rand.h>

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void MIDIPlayer::setupMIDI () {
  MIDI.begin();
  MIDI.setHandleNoteOn(MyHandleNoteOn);
  MIDI.setHandleNoteOff(MyHandleNoteOff);
}

void MIDIPlayer::play () {
  stop ();
  state = Playing;
  io = defaultMIDIRecHandle ();
  io.seek (0);
  event_time = nowTime() + readLong ();
}
  
void MIDIPlayer::playRandom () {
  stop ();
//  randSeed (mozziMicros ());  -- hangs?!
  state = PlayingRandom;
}

void MIDIPlayer::stop () {
  io.close ();
  state = Stopped;
}

void MIDIPlayer::record () {
  stop ();
  state = Recording;
  event_time = 0;
  io = defaultMIDIRecHandle ();
  io.seek (0);
}

void MIDIPlayer::update () {
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
    uint32_t now = nowTime();
    if (io.available()) {
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
          // NOTE: To handle playback of (almost) universal MIDI files, we we will have to a) handle the header, and b) handle/skip over META events (mtype 0xFF)
          // for now, we're just playing back what we record, while keeping the storage format close to the MIDI specs.
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
      if (io.size () > 0) { // Loop, unless there is no file to play.
        io.seek (0);
        event_time = now + 1000 + readLong ();
      } else {
        playRandom ();
      }
    }
  }
  
  if (state == PlayingRandom) {
    uint32_t now = nowTime();
    if (now - event_time > 1000) {
      MyHandleNoteOn (1, rand (20) + 77, 100);
      event_time = now;
    }
  }
}

void MIDIPlayer::writeLong (uint32_t val) {
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

uint32_t MIDIPlayer::readLong () {
  byte in;
  uint32_t ret = 0;
  byte pos = 0;
  do {
    in = io.read ();
    ret = (ret << 7) + (in & 0x7F);
  } while ((in & 0x80) && (++pos < 4)); // The length check may seem overcautios on first glance, but on end of file, io.read() will return -1, i.e. always have the 0x80 bit.
  return ret;
}

uint32_t MIDIPlayer::getDelta () {
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

uint32_t MIDIPlayer::nowTime () const {
    return millis ();
}

byte MIDIPlayer::MidiMessageLength (byte mtype) const {
  // TODO: This is correct only for the MIDI events we record ourselves. not for everything else.
  if (mtype == midi::ProgramChange || mtype == midi::AfterTouchChannel) return 2;
  return 3;
}


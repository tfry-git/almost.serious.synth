#include "midiplayer.h"

#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_RingBuffer.h>
#include <mozzi_rand.h>

#define SERIAL_INTERFACE Serial1

/** This class wraps the real serial interface to use, but adds a buffer that we can use to feed
 *  MIDI events (esp. from file) to the MIDI library.
 */
class MIDISideBuffer {
public: // Arduino Serial API
    void begin (int inBaudrate) {
      SERIAL_INTERFACE.begin (inBaudrate);
    }

    int available() const {
      if (buffer.isEmpty()) return SERIAL_INTERFACE.available();
      return buffer.getLength();
    }

    void write(uint8_t inData) {
      SERIAL_INTERFACE.write (inData);
    }

    void writeBuffer (uint8_t inData) {
      buffer.write (inData);
    }

    uint8_t read() {
      if (buffer.isEmpty()) return SERIAL_INTERFACE.read();
      return buffer.read();
    }
private:
    midi::RingBuffer<uint8_t, 128> buffer;
};

MIDISideBuffer midi_side_buffer;

MIDI_CREATE_INSTANCE(MIDISideBuffer, midi_side_buffer, MIDI);

byte MidiMessageLength (byte mtype) {
  // TODO: This is correct only for the MIDI events we record ourselves. not for everything else.
  if (mtype == midi::ProgramChange || mtype == midi::AfterTouchChannel) return 2;
  return 3;
}

void MIDIPlayer::setupMIDI () {
  MIDI.begin();
  MIDI.setHandleNoteOn(MyHandleNoteOn);
  MIDI.setHandleNoteOff(MyHandleNoteOff);
}

void MIDIPlayer::play () {
  stop ();
  state = Playing;
  io = defaultMIDIRecHandle ();
  infile.load (io);
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
    while (infile.doNextEvent (now)) {};
    if (!io.available ()) {
      if (io.size () > 0) { // Loop, unless there is no file to play.
        play ();
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


void MIDIPlaybackFile::load (File f) {
  io = f;
  io.seek (0);
  next_event_time = nowTime ();

  if (io.available () >= 14) {  // size of MIDI header
    char buf[14];
    io.read (buf, 14);
    if (buf[0] == 'M' && buf[1] == 'T' && buf[2] == 'h' && buf[3] == 'd') {  // MIDI header
      // TODO: parse delta time specification
    } else {
      io.seek (0); // ignore, and try to interpret as naked track, below
    }
  }

  handleTrackHeader ();
}

void MIDIPlaybackFile::handleTrackHeader () {

  if (io.available () >= 8) {
    byte buf[8];
    io.read (buf, 8);
    if (buf[0] == 'M' && buf[1] == 'T' && buf[2] == 'r' && buf[3] == 'k') {  // MIDI track
      tracklen = buf[4] << 24 + buf[5] << 16 + buf[6] << 8 + buf[7];
    } else {
      io.seek (io.position () - 8); // go back, and try to interpret as naked event stream
      tracklen = io.size () - io.position ();
    }
  }
  trackstart = io.position ();

  advance ();
}

bool MIDIPlaybackFile::doNextEvent (uint32_t now) {
  if (now < next_event_time) return false;

  byte in = io.read (); // This should be the command byte
  while (!(in & 0x80)) { // But we got something else? Fast forward to the next elegible byte, and _try_ to make some sense from there.
    // Which is not a really good strategy, either, as - contrary to a live stream - we will probably probably get screwed by a delta time, instead...
    if (atEndOfTrack ()) {
      handleTrackHeader ();
      return false;
    }
    in = io.read ();
  }
  if (in == 0xFF) {  // Meta event. we'll have to eat that, as the MIDI library does not handle it
    in = io.read ();
    if (in == 0x2F) {      // end of track
      io.read ();
      handleTrackHeader ();
      return false;
    } else if (in == 0xFF) {   // set tempo
      // TODO We should really handle this...
    }
    byte len = io.read ();
    io.seek (io.position () + len);
  } else {
      // TODO: We should really offload the following to the MIDI library, by writing it to a fake in buffer.
      // BUT why isn't this working?
/*    midi_side_buffer.writeBuffer (in);
    while (!MIDI.read ()) {
      if (atEndOfTrack ()) break;
      midi_side_buffer.writeBuffer (io.read ());
    } */
    byte mtype = in & 0xF0;
    byte buf[16];
    byte pos = 0;
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
  }

  advance ();
  return true;
}

bool MIDIPlaybackFile::atEndOfTrack () {
  return (trackstart + tracklen < io.position () || !io.available ());
}

void MIDIPlaybackFile::advance () {
  if (!atEndOfTrack ()) next_event_time += readVarLong ();
  else if (io.available ()) handleTrackHeader ();
}

uint32_t MIDIPlaybackFile::readVarLong () {
  byte in;
  uint32_t ret = 0;
  byte pos = 0;
  do {
    in = io.read ();
    ret = (ret << 7) + (in & 0x7F);
  } while ((in & 0x80) && (++pos < 4)); // The length check may seem overcautious on first glance, but on end of file, io.read() will return -1, i.e. always have the 0x80 bit.
  return ret;
}

uint32_t MIDIPlaybackFile::nowTime () const {
  return millis ();
}


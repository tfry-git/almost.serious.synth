#include "midiplayer.h"

#include <MIDI.h>
#include <midi_Defs.h>
#include <mozzi_utils.h>
#include <mozzi_rand.h>
#include <MozziGuts.h>
#include "display.h"
#include "util.h"

#define SERIAL_INTERFACE Serial1

/** This class wraps the real serial interface to use, but adds a buffer that we can use to feed
 *  MIDI events (esp. from file) to the MIDI library.
 */
/*class MIDISideBuffer {
public: // Arduino Serial API
    void begin (int inBaudrate) {
      SERIAL_INTERFACE.begin (inBaudrate);
    }

    int available() const {
//      if (buffer.isEmpty()) return SERIAL_INTERFACE.available();
      return buffer.getLength();
    }

    void write(uint8_t inData) {
      SERIAL_INTERFACE.write (inData);
    }

    void writeBuffer (uint8_t inData) {
      buffer.write (inData);
    }

    uint8_t read() {
//      if (buffer.isEmpty()) return SERIAL_INTERFACE.read();
      return buffer.read();
    }
private:
    midi::RingBuffer<uint8_t, 128> buffer;
} midi_side_buffer;
MIDI_CREATE_INSTANCE(MIDISideBuffer, midi_side_buffer, MIDI); */
MIDI_CREATE_INSTANCE(HardwareSerial, SERIAL_INTERFACE, MIDI);

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

void MIDIPlayer::play (const char *file) {
  stop ();
  state = Playing;
  if (file[0] == '\0') {
    if (!io) io = defaultMIDIRecHandle ();
    else openMidiFile (io.name ()); // it was closed, above
  } else {
    io = openMidiFile (file);
  }
  display_detail ("Playing", getFileName (io));
  infile.load (io);
}
  
void MIDIPlayer::playRandom () {
  stop ();
//  randSeed (mozziMicros ());  -- hangs?!
  state = PlayingRandom;
  display_detail ("Playing", "random notes");
}

void MIDIPlayer::stop () {
  if (state == Recording) {
    uint32_t size = io.size ();
    byte buf[4];
    for (int8_t pos = 3; pos >= 0; --pos) {
      buf[pos] = size & 0xFF;
      size = size >> 8;
    }
    io.seek (14 + 4); // The position where the track length is stored.
    io.write (buf, 4);
  }
  io.close ();
  state = Stopped;
}

void MIDIPlayer::record () {
  stop ();
  state = Recording;
  event_time = 0;
  io = defaultMIDIRecHandle ();
  io.seek (0);
  byte head[14] = {0x4d, 0x54, 0x68, 0x64, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x01, 0xe7, 0x28};  // MIDI header chunk, Format 0, one track, millisecond based
  io.write (head, 14);
  byte trackhead[] = {0x4d, 0x54, 0x72, 0x6b, 0x00, 0xff, 0xff, 0xff}; // MIDI track header chunk. We lie about the length, as we don't know it, yet.
  io.write (trackhead, 8);
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
    if (!infile.isFinished ()) {
      infile.processEvents ();
    } else {
      if (io.size () > 0) { // Loop, unless there is no file to play.
        play (io.name ());
      } else {
        playRandom ();
      }
    }
  }
  
  if (state == PlayingRandom) {
    uint32_t now = mozziMicros();
    if (now - event_time > 2000000ul) {
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
  num_tracks = 1;
  format2 = true;

  if (io.available () >= 14) {  // size of MIDI header
    byte buf[14];
    io.read (buf, 14);
    if (buf[0] == 'M' && buf[1] == 'T' && buf[2] == 'h' && buf[3] == 'd') {  // MIDI header
      if (buf[12] & 0x80) {
        // SMPTE delta time spec - slightly verbose calculation, but it makes my brain hurt
        uint8_t frames_per_second = -(signed char) buf[12];
        uint8_t ticks_per_frame = buf[13];
        uint16_t ticks_per_second = frames_per_second * ticks_per_frame;
        micros_per_tick = 1000000ul / ticks_per_second;
        // SMPTE indexed files must not send set tempo events, but just in case, we'll initalize the tempo spec to 120 bpm,
        // so we can still handle set tempo events.
        ticks_per_beat = 500000ul / micros_per_tick;
      } else {
        ticks_per_beat = ((uint32_t) buf[12] << 8) + (uint32_t) buf[13];
        micros_per_tick = 500000ul / ticks_per_beat;   // 500000 micros per beat = 120 beats per minute
      }
      if (buf[9] == 1) { // format 1 file
        num_tracks = min (MAX_MIDI_TRACKS, buf[11]);//u8ranged (min (MAX_MIDI_TRACKS, ((uint16_t) buf[10] << 8 + buf[11])));
        format2 = false;
      }
      // NOTE that we do not update track count in case of format 2 files: As there is only one _simultaneous_ track, we don't need any fancy bookkeeping
      // and just skip over any track ends / headers found in the middle of the file.
      // For the same reason, we simply treat format0 as format2
    } else {
      io.seek (0); // ignore, and try to interpret as naked track, below
      micros_per_tick = 1000ul;  // Assume millisecond based, 120 bpm
      ticks_per_beat = 500000ul / micros_per_tick;
    }
  }

  for (uint8_t i = 0; i < num_tracks; ++i) {
    if (i > 0) {
      io.seek (tracks[i-1].ioend);
    }
    Track &track = tracks[i];
    track.running_status = 0;
    track.ioend = io.size () - 1; // correct value will be set in handleTrackHeader, but we need an upper bound!
    track.iopos = io.position ();
    track.buflen = 0;
    track.bufpos = 0;
    track.next_event_time = 0;
    handleTrackHeader (track);
  }

  current_tick = 0;
  current_tick_time = mozziMicros ();
}

void MIDIPlaybackFile::handleTrackHeader (Track &track) {

  if (available (track) >= 8) {
    byte buf[8];
    for (int i = 0; i < 8; ++i) buf[i] = read (track);
    if (buf[0] == 'M' && buf[1] == 'T' && buf[2] == 'r' && buf[3] == 'k') {  // MIDI track
      track.ioend = position (track) + ((uint32_t) buf[4] << 24) + ((uint32_t) buf[5] << 16) + ((uint32_t) buf[6] << 8) + (uint32_t) buf[7];
    } else {
      // go back, and try to interpret as naked event stream (unfortunately that includes discarding the buffer, but _should_ not happen on valid MIDI files
      track.iopos = position (track) - 8;
      track.ioend = io.size () - 1;
      track.buflen = 0;
      track.bufpos = 0;
    }
  } else {
    track.ioend = io.size () - 1;
  }

  advance (track);
}

void MIDIPlaybackFile::processEvents () {
  uint32_t elapsed_ticks = (mozziMicros () - current_tick_time) / micros_per_tick;
  current_tick += elapsed_ticks;
  current_tick_time += elapsed_ticks * micros_per_tick;  // not simply using "now", as we might be half-way into the next tick

  bool any_event = false; // TODO: REMOVE ME: temporary same loop buffer refill strategy
  for (uint8_t i = 0; i < num_tracks; ++i) {
    Track &track = tracks[i];
    bool first = true;
    while (current_tick >= track.next_event_time) {
      first = false;
      if (atEndOfTrack (track)) break;
      if (!doNextEvent (track)) break;
    }
    if (!first) {
      any_event = true;
      if (atEndOfTrack (track)) {  // if a track has finished playing, remove it.
        for (uint8_t j = i+1; j < num_tracks; ++j) {
          tracks[j-1] = tracks[j];
        }
        --num_tracks;
        --i;
      }
    }
  }
  if (!any_event) {
    for (uint8_t i = 0; i < num_tracks; ++i) {
      Track &track = tracks[i];
      if (available (track) < (MIDI_TRACK_BUFSIZE / 2)) {
        fillBuffer (track);
        break;
      }
    }
  }
}

bool MIDIPlaybackFile::doNextEvent (Track &track) {
   // TODO: I'd really like to offload most of the parsing (well, apart from Meta events) to the midi library. However, trying to just sneak it into the MIDI input buffer not not seem to work (hangs)
/*    midi_side_buffer.writeBuffer (in);
    while (!MIDI.read ()) {
      if (atEndOfTrack (track)) break;
      midi_side_buffer.writeBuffer (io.read ());
    } */

  byte in = peek (track);  // This should be the command byte
  if (!(in & 0x80)) { // Unless it's a running status. Whoever introduced that silly idea into the standard?
    in = track.running_status;
  } else {
    in = read (track);
    track.running_status = in;
  }

  // handle events
  if (in == 0xFF) {  // Meta event. we'll have to eat that, as the MIDI library does not handle it
    in = read (track);
    if (in == 0x2F) {      // end of track
      read (track);
      if (format2) {
        track.ioend = io.size () - 1;
        handleTrackHeader (track);
      } else {
        track.ioend = position (track);
      }
      return false;
    } else if (in == 0x51) {   // set tempo
      read (track);
      uint32_t micros_per_beat = ((uint32_t) read (track) << 16) + ((uint32_t) read (track) << 8) + (uint32_t) read (track);
      micros_per_tick = micros_per_beat / ticks_per_beat;
    } else if (in == 0x54) {  // SMPTE offset - handle like a delta time
      read (track);
      // NOT correct! We assume millisecond ticks, here (frames and frame-fraction bits), which is not a bad guess, but no more than that.
      track.next_event_time += (uint32_t) read (track) * (3600000000ul / micros_per_tick) + (uint32_t) read (track) * (60000000ul / micros_per_tick) + (uint32_t) read (track) * (1000000ul / micros_per_tick) + (uint32_t) read (track)*40ul + read (track);
    } else {
      byte len = read (track);
      for (byte i = 0; i < len; ++i) {
        read (track); // actually, all we want to do is skip, but probably not worth optimizing this.
      }
    }
  } else if (in >= 0xF0) { // System events -- we'll just skip those
    if (in == 0xF0) {
      byte in = read (track); // skip vendor byte
      while ((in = read (track)) != 0xF7) { // look for termination byte
        if (atEndOfTrack (track)) break;
      }
    } else if (in == 0xF2) {
      read (track);
      read (track);
    } else if (in == 0xF3) {
      read (track);
    } else {
      // no further bytes to read
    }
  // regular events
  } else {
    byte mtype = in & 0xF0;
    byte buf[16];
    byte pos = 0;
    buf[0] = in;
    while (++pos < MidiMessageLength (mtype)) { // read all data bytes
      // NOTE: To handle playback of (almost) universal MIDI files, we we will have to a) handle the header, and b) handle/skip over META events (mtype 0xFF)
      // for now, we're just playing back what we record, while keeping the storage format close to the MIDI specs.
      buf[pos] = read (track);
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

  advance (track);
  return true;
}

byte MIDIPlaybackFile::read (Track &track) {
  if (!track.buflen) fillBuffer (track);
//  if (!track.buflen) return -1;
  --track.buflen;

  if (++track.bufpos >= MIDI_TRACK_BUFSIZE) track.bufpos = 0;
  return track.iobuf[track.bufpos];
}

byte MIDIPlaybackFile::peek (Track &track) {
  if (!track.buflen) fillBuffer (track);
  return track.iobuf[(track.bufpos + 1) % MIDI_TRACK_BUFSIZE];
}

void MIDIPlaybackFile::fillBuffer (Track &track) {
  uint8_t from = (track.bufpos + track.buflen + 1) % MIDI_TRACK_BUFSIZE;
  uint8_t to = ((from < track.bufpos) ? track.bufpos : MIDI_TRACK_BUFSIZE) - 1;
  if (io.position () != track.iopos) io.seek (track.iopos);
  uint8_t len = io.read (&track.iobuf[from], to - from + 1);
  track.iopos += len;
  track.buflen += len;
}

void MIDIPlaybackFile::advance (Track &track) {
  if (!atEndOfTrack (track)) {
    track.next_event_time += readVarLong (track);
  }
  else if (format2 && io.available ()) handleTrackHeader (track);
}

uint32_t MIDIPlaybackFile::readVarLong (Track &track) {
  byte in;
  uint32_t ret = 0;
  byte pos = 0;
  do {
    in = read (track);
    ret = (ret << 7) + (in & 0x7F);
  } while ((in & 0x80) && (++pos < 4)); // The length check may seem overcautious on first glance, but on end of file, io.read() will return -1, i.e. always have the 0x80 bit.
  return ret;
}


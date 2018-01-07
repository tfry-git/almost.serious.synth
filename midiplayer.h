#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include "storage.h"

void MyHandleNoteOn (byte channel, byte pitch, byte velocity);
void MyHandleNoteOff(byte channel, byte pitch, byte velocity);

#define MAX_MIDI_TRACKS 16u  // for now we use static allocation of MIDI tracks (for format 1 playback). NOTE this is track _per_ file

/** Wraps a MIDI file to be played back. Importantly, it takes care of parsing (well - mostly skipping, for now) the file
 *  header and track headers.
 */
class MIDIPlaybackFile {
public:
  void load (File io);
  void processEvents ();
  bool atEndOfTrack ();
  inline bool isFinished () {
    if (format2) return (!io.available ());
    return (num_tracks == 0);
  }
private:
  uint32_t nowTime () const;
  /** Read a MIDI variable length entry from file. */
  uint32_t readVarLong ();
  uint32_t ticksToMicros (uint32_t ticks) const {
    return ticks * micros_per_tick;
  }
  File io;
  uint32_t num_tracks;
  bool format2;
  uint32_t micros_per_tick;
  uint32_t ticks_per_beat;

  struct Track {
    uint32_t trackend;
    uint32_t trackpos;
    uint32_t next_event_time;
  } tracks[MAX_MIDI_TRACKS];
  void handleTrackHeader (Track &track);
  inline bool atEndOfTrack (Track &track) {
    return (io.position () >= track.trackend);
  }
  bool doNextEvent (Track &track);
  void advance (Track &track);
};

/** This class is meant to handle recording and playback of MIDI events. */
class MIDIPlayer {
public:
  void setupMIDI ();
  void play (const char *file = "");
  void playRandom ();
  void stop ();
  void record ();
  /** To be called periodically for processing (handling MIDI IN, playback, etc. */
  void update ();
  bool isPlaying () const {
    return (state == Playing || state == PlayingRandom);
  }
  bool isRecording () const {
    return (state == Recording);
  }
private:
  /** Write a MIDI variable length entry. Max four bytes, seven bits per byte 0x80 to signify continuation */
  void writeLong (uint32_t val);
  /** Get delta time since last events, and update timestamp */
  uint32_t getDelta ();
  /** Current time. TODO: should return in MIDI ticks. */
  uint32_t nowTime () const;
  enum PlayerState {
    Stopped,
    Playing,
    PlayingRandom,
    Recording
  };
  uint32_t event_time;  // timestamp of the previous (for recording) or next (for playback) event in the stream
  PlayerState state = Stopped;
  File io;
  MIDIPlaybackFile infile;
};

extern MIDIPlayer player;

#endif

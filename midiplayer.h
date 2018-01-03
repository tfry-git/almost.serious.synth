#ifndef MIDIPLAYER_H
#define MIDIPLAYER_H

#include "storage.h"

void MyHandleNoteOn (byte channel, byte pitch, byte velocity);
void MyHandleNoteOff(byte channel, byte pitch, byte velocity);

/** This class is meant to handle recording and playback of MIDI events. */
class MIDIPlayer {
public:
  void setupMIDI ();
  void play ();
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
  /** Read a MIDI variable length entry from file. */
  uint32_t readLong ();
  /** Get delta time since last events, and update timestamp */
  uint32_t getDelta ();
  /** Current time. TODO: should return in MIDI ticks. */
  uint32_t nowTime () const;
  byte MidiMessageLength (byte mtype) const;
  enum PlayerState {
    Stopped,
    Playing,
    PlayingRandom,
    Recording
  };
  uint32_t event_time;  // timestamp of the previous (for recording) or next (for playback) event in the stream
  PlayerState state = Stopped;
  File io;
};

extern MIDIPlayer player;

#endif

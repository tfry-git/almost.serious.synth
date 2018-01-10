#ifndef STORAGE_H
#define STORAGE_H

/** Simple abstraction layer around the permanent storage device (if any) */

//#define USE_SDFAT  // Use Bill Greiman's SdFat library instead of the Arduino SD library
#ifdef USE_SDFAT
#include <SdFat.h>
#else
#include <SD.h>
#endif

void setup_storage();
File defaultVoiceFileHandle ();
File defaultMIDIRecHandle ();
File openMidiDirectory ();
File openVoiceDirectory ();
File openVoiceFile (const char *name);
File openMidiFile (const char *name);

#endif


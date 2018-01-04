#ifndef STORAGE_H
#define STORAGE_H

/** Simple abstraction layer around the permanent storage device (if any) */

#include <SD.h>

void setup_storage();
File defaultVoiceFileHandle ();
File defaultMIDIRecHandle ();
File openMidiDirectory ();
File openVoiceDirectory ();
File openVoiceFile (const char *name);
File openMidiFile (const char *name);

#endif


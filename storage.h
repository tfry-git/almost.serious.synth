#ifndef STORAGE_H
#define STORAGE_H

/** Simple abstraction layer around the permanent storage device (if any) */

#include <SD.h>

void setup_storage();
File defaultVoiceReadHandle ();
File defaultVoiceSaveHandle ();
File defaultMIDIRecHandle ();

#endif


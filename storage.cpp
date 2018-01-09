#include "storage.h"

#define VOICES_DIRECTORY "/voices"
#define MIDI_DIRECTORY "/midi"

#include <SPI.h>
#include "display.h"

bool SD_storage_available = false;

void setup_storage () {
  SPI.setModule (2);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  if (!SD.begin(PB12)) {
    display_detail ("SD init fail", ":-(");
    delay (1000);
  } else {
    SD.mkdir (VOICES_DIRECTORY);
    SD.mkdir (MIDI_DIRECTORY);
    SD_storage_available = true;
  }
}


File defaultVoiceFileHandle () {
  return openVoiceFile ("");
}

File openVoiceFile (const char* name) {
  if (!SD_storage_available) return File();
  char buf[32];
  strcpy (buf, VOICES_DIRECTORY);

  if (name[0] == '\0') { // No name specified: Use first file in dir
    File dir = SD.open(buf);
    File dummy = dir.openNextFile();
    dir.close ();
    if (!dummy) return File ();
    strcat (buf, "/");
    strcat (buf, dummy.name ());
    dummy.close ();
    return SD.open(buf, FILE_WRITE);
  } else {
    strcat (buf, "/");
    strcat (buf, name);
  }

  return SD.open(buf, FILE_WRITE);
}

File openVoiceDirectory () {
  if (!SD_storage_available) return File();
  return SD.open(VOICES_DIRECTORY);
}

File defaultMIDIRecHandle () {
  return openMidiFile ("");
}

File openMidiDirectory () {
  if (!SD_storage_available) return File();
  return SD.open(MIDI_DIRECTORY);
}

File openMidiFile (const char* name) {
  if (!SD_storage_available) return File();
  if (name[0] == '\0') return SD.open (MIDI_DIRECTORY "/0.mid", FILE_WRITE);
  char buf[32] = "";
  strcat (buf, MIDI_DIRECTORY);
  strcat (buf, "/");
  strcat (buf, name);
  return SD.open(buf, FILE_WRITE);
}


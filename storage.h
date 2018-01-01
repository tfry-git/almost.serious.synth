#ifndef STORAGE_H
#define STORAGE_H

/** Simple abstraction layer around the permanent storage device (if any) */

#include <SD.h>
bool SD_storage_available = false;

#define VOICES_DIRECTORY "/voices"
#define MIDI_DIRECTORY "/midi"

void setup_storage() {
  SPI.setModule (2);
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  if (!SD.begin(PB12)) {
    display_detail ("SD init fail", ":-(");
    delay (1000);
  } else {
    SD.mkdir (VOICES_DIRECTORY);
    SD.mkdir (MIDI_DIRECTORY);
    SD_storage_available = true;
  }
}

File defaultVoiceReadHandle () {
  if (!SD_storage_available) return File();
  return SD.open(VOICES_DIRECTORY "/0.voc");
}

File defaultVoiceSaveHandle () {
  if (!SD_storage_available) return File();
  return SD.open(VOICES_DIRECTORY "/0.voc", FILE_WRITE);
}

File defaultMIDIRecHandle () {
  if (!SD_storage_available) return File();
  return SD.open(MIDI_DIRECTORY "/0.mid", FILE_WRITE);
}

#endif


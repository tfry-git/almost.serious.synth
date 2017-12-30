#include <SD.h>
File root;

#define VOICES_DIRECTORY "/voices"

void setup_storage() {
  SPI.setModule (2);
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  if (!SD.begin(PB12)) {
      display_detail ("SD init fail", ":-(");
      delay (1000);
  }
  SD.mkdir (VOICES_DIRECTORY);
}

File defaultVoiceReadHandle () {
  return SD.open(VOICES_DIRECTORY "/0.voc");
}

File defaultVoiceSaveHandle () {
  return SD.open(VOICES_DIRECTORY "/0.voc", FILE_WRITE);
}


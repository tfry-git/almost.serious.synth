#include "userinput.h"

#if defined (POT_AS_TUNER)

#include <MozziGuts.h>
uint16_t potentiometer_center;
int32_t potentiometer_microticks;

void setup_updown () {
    pinMode (POT_AS_TUNER_PIN, INPUT_ANALOG);
    potentiometer_center = analogRead (POT_AS_TUNER_PIN);
    potentiometer_microticks = 0;
}

extern int16_t asyncAnalogRead (uint8_t pin);
int8_t read_updown () {
    int32_t current = asyncAnalogRead (POT_AS_TUNER_PIN) - potentiometer_center;
    uint32_t abscur = abs (current);
    if (abscur > POT_AS_TUNER_FAST_THRESHOLD) { // Increasing accelaration (quadratically) above "fast" threshold
        current *= (abscur - POT_AS_TUNER_FAST_THRESHOLD) / 8;
    } else {
        if (abscur < POT_AS_TUNER_THRESHOLD) current = 0;   // idle below "min" threshold
    }
    
    potentiometer_microticks += current;
    int8_t ret = potentiometer_microticks >> POT_AS_TUNER_LSHIFT;
    potentiometer_microticks -= ret << POT_AS_TUNER_LSHIFT;
    return ret;
}

#endif

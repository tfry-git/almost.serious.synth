#include "userinput.h"

#if defined (POT_AS_TUNER)

#include <MozziGuts.h>
static uint16_t center_ud;
static uint16_t center_lr;
static uint32_t last_handled_iter;

void setup_updown () {
    pinMode (POT_AS_TUNER_PIN_UD, INPUT_ANALOG);
    pinMode (POT_AS_TUNER_PIN_LR, INPUT_ANALOG);
    center_ud = analogRead (POT_AS_TUNER_PIN_UD);
    center_lr = analogRead (POT_AS_TUNER_PIN_LR);
    last_handled_iter = 0;
}

extern int16_t asyncAnalogRead (uint8_t pin);

static int8_t readPot (int32_t current) {
    uint32_t abscur = abs (current);
    if (abscur < POT_AS_TUNER_THRESHOLD) return 0;

    uint32_t now = millis ();
    uint32_t elapsed = now - last_handled_iter;

    // When the pot has just left the center for the first time in a while, immediately signal one tick.
    bool fresh = elapsed > 500;
    if (fresh) {
        last_handled_iter = now;
        return (current > 0);
    }

    // For all subsequent calls, the return value depends on the position of the pot
    int8_t ret = (current * elapsed) >> POT_AS_TUNER_LSHIFT;
    if (abscur > POT_AS_TUNER_FAST_THRESHOLD) { // Increasing accelaration (quadratically) above "fast" threshold
      ret *= (abscur / POT_AS_TUNER_FAST_THRESHOLD) + 1;
    }

    if (ret != 0) {
        last_handled_iter = now;
    }
    return ret;
}

int8_t read_updown () {
    return readPot (asyncAnalogRead (POT_AS_TUNER_PIN_UD) - center_ud);
}

int8_t read_leftright () {
    return readPot ((asyncAnalogRead (POT_AS_TUNER_PIN_LR) - center_lr) >> 1);
}

#endif

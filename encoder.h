#ifndef ENCODER_H
#define ENCODER_H

/* Reading from a rotary encoder, with software debouncing */

// Input pins. NOTE That these _must_ support intterupts, _and_ interrupts must not interfere with other pins
// (e.g. on STM32F103C8T6, PAx will interfere with PBx)
#define ENCODER_PIN_A PA1
#define ENCODER_PIN_B PA2
#define BOUNCE_TIMEOUT 2  // ms

// Encoder needs to check for updates more frequently than control rate.
#define HIGH_FREQUENCY_HOOK update_encoder

volatile uint16_t pin_a_change_time;
volatile uint16_t pin_b_change_time;
volatile bool encoder_change_pending;
uint16_t last_handled_change_time;
int8_t encoder_micro_tick;

void encoderTickA () {
  // record that _something_ has happened with the encoder...
  encoder_change_pending = true;
  // ... and when - so we can discard bounces
  pin_a_change_time = millis ();
}

void encoderTickB () {
  encoder_change_pending = true;
  pin_b_change_time = millis ();
}

// Taken from https://www.circuitsathome.com/mcu/reading-rotary-encoder-on-arduino/ by Oleg Mazurov
// (slightly modified to work be more portable, and to add debouncing)
// call this frequently to make sure to catch all encoder changes. Call read_encoder() to read number of
// ticks since last read.
void update_encoder () {
  // unfortunately, trying to keep track of the pin state in the two encoderTickX()-functions (without reading the pins, manually)
  // does not work, reliably. So we do need to do digitalRead() on the pins, here. _But_ if we know, no pin changed since the last iteration,
  // we can simply skip that.
  if (!encoder_change_pending) return;
  encoder_change_pending = false;
  uint16_t nowt = millis ();
  if (nowt < last_handled_change_time) { // handle timer overflow
    pin_a_change_time = pin_b_change_time = 0;
  }
  last_handled_change_time = nowt;

  uint8_t pin_a_val = 0;
  if (last_handled_change_time - pin_a_change_time < BOUNCE_TIMEOUT) {
    // if the pin level has been fluctuating, recently, we always assume the contact is closed (corresponding to 0 reading!)...
    // ... but we should make sure to revisit the pin after the bounce timeout
    encoder_change_pending = true;
  } else {
    // Note that we don't shy away from "expensive but portable" digitalRead(), here:
    // - We'll only get to this part of the code, if something _has_ changed, and
    // - We'll not get here more often than every BOUNCE_TIMEOUT milliseconds
    // But replace with a faster read, if you prefer.
    pin_a_val = digitalRead (ENCODER_PIN_A);
  }
  uint8_t pin_b_val = 0;
  if (last_handled_change_time - pin_b_change_time < BOUNCE_TIMEOUT) {
    encoder_change_pending = true;
  } else {
    pin_b_val = digitalRead (ENCODER_PIN_B) << 1;
  }

  // table of valid (1/-1) or invalid (0) transitions from previous state to next state.
  // where previous state is the two higher bits of the array position, and next state is the two lower bits
  int8_t enc_states[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
  static uint8_t old_AB = 0;

  old_AB <<= 2;                   // previous state as high bits
  old_AB |= pin_a_val | pin_b_val;
  encoder_micro_tick += enc_states[(old_AB & 0x0f)];
}

int8_t read_encoder () {
  int8_t ret = encoder_micro_tick / 4;
  if (ret != 0) {
    encoder_micro_tick = 0;
    // TODO: What the??? Somehow the version below will cause audible clicks, even while the encoder is not turning at all.
    // In practice, the line above, while not quite correct, will not be noticeably different.
//      encoder_micro_tick -= ret * 4;
  }
  return ret;
}

void setup_encoder() {
  pinMode (ENCODER_PIN_A, INPUT_PULLUP);
  pinMode (ENCODER_PIN_B, INPUT_PULLUP);
  attachInterrupt (ENCODER_PIN_A, encoderTickA, CHANGE);
  attachInterrupt (ENCODER_PIN_B, encoderTickB, CHANGE);
  pin_a_change_time = pin_b_change_time = last_handled_change_time = millis ();
  encoder_micro_tick = 0;
}

#endif


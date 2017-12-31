#ifndef MATRIX_H
#define MATRIX_H

#define DEBOUNCE_DELAY 100  // Only one valid keypress will be detected per given number of ms. Don't set this too low for a cheap button matrix

/* Read from a button matrix
 *  Adjustable pin definitions at the bottom of this file. */
template<uint8_t ROWS, uint8_t COLS> class KeypadMatrix {
public:
    void init (uint8_t rowpins[ROWS], uint8_t colpins[COLS]) {
        for (uint8_t i = 0; i < ROWS; ++i) {
            KeypadMatrix::rowpins[i] = rowpins[i];
            pinMode (rowpins[i], INPUT_PULLUP);
        }
        for (uint8_t i = 0; i < COLS; ++i) {
            KeypadMatrix::colpins[i] = colpins[i];
            pinMode (colpins[i], OUTPUT);
            digitalWrite (colpins[i], LOW);
        }
        
    }
    /** return the current key (numbered from left to right, top to bottom, starting at 0), or -1 if no key is pressed.
     * This function is totally not optimized, uses lots of digitalRead()s. */
    int read () {
      uint32_t now = millis ();
      if (now < last_valid_read || (now > last_valid_read + DEBOUNCE_DELAY)) {
        int res = _read ();
        if (res >= 0) {
          last_valid_read = now;
        }
        return res;
      }
      return -1;
    }
private:
    int _read() {
        int row = -1;
        for (uint8_t i = 0; i < ROWS; ++i) {
            if (!digitalRead (rowpins[i])) {
                row = i;
                break;
            }
        }
        if (row < 0) return -1;

        uint8_t col = COLS - 1;
        for (uint8_t i = 0; i < col; ++i) {
            digitalWrite (colpins[i], HIGH); // turn off columns one by one, to find out, if it's the right one
            if (digitalRead (rowpins[row])) {
                col = i;
                break;
            }
        }
        // Pull columns low again, for the next iteration...
        for (uint8_t i = 0; i <= col; ++i) {
            digitalWrite (colpins[i], LOW);
        }
        // ... and for a verification of the read (if the button is still bouncing, we may have mis-detected the column).
        if (!digitalRead (rowpins[row])) {
            return (row * COLS + col);
        }
        return -1;
    }
    uint8_t rowpins[ROWS];
    uint8_t colpins[COLS];
    uint32_t last_valid_read = 0;
};


KeypadMatrix<4,4> keypad;
void setup_keypad () {
  uint8_t keyrows[4] = {PA7,PA6,PA5,PA4};  // for some strange reason, I had a _lot_ of noise on PA11 on my board. Perhaps related to I2C on PB11?
  uint8_t keycols[4] = {PB11,PB10,PB1,PB0};
  keypad.init (keyrows, keycols);
}

#endif


/* The waveforms to use. Note that for the time being, we support exactly two distinct table sizes (TABLE_SIZE_A, TABLE_SIZE_B).
Lower table sizes, help to keep the sketch size small, but are not as pure (which may not even be a bad thing)
*/
#include <tables/sin2048_int8.h>
#include <tables/saw2048_int8.h>
#include <tables/triangle2048_int8.h>
#include <tables/square_no_alias_2048_int8.h>
#include <tables/whitenoise8192_int8.h>
#include <tables/chum9_int8.h>  // TODO: replace with something better
#include "aah8192_int8.h"
#define NUM_TABLES 7
#define TABLE_SIZE_A 2048
#define TABLE_SIZE_B 8192
const int16_t WAVE_TABLE_SIZES[NUM_TABLES] = {TABLE_SIZE_A, TABLE_SIZE_A, TABLE_SIZE_A, TABLE_SIZE_A, TABLE_SIZE_B, TABLE_SIZE_B, TABLE_SIZE_B};
const int8_t* WAVE_TABLES[NUM_TABLES] = {SQUARE_NO_ALIAS_2048_DATA, SIN2048_DATA, SAW2048_DATA, TRIANGLE2048_DATA, CHUM9_DATA, AAH8192_DATA, WHITENOISE8192_DATA};
const int8_t FREQ_SHIFT[NUM_TABLES] = {0, 0, 0, 0, 8, 9, 0}; // Some "waveforms" are actually samples that should be played at a much lower rate for the same frequency
#define IS_NOISE_TABLE(x) (x / NUM_TABLES >= 6)

#include <tables/waveshape_chebyshev_3rd_256_int8.h>
#include <tables/waveshape_chebyshev_5th_256_int8.h>
#define NUM_SHAPES 3
WaveShaper<char> wshape_chebyshev3 (CHEBYSHEV_3RD_256_DATA);
WaveShaper<char> wshape_chebyshev5 (CHEBYSHEV_5TH_256_DATA);
WaveShaper<char>* WAVE_SHAPERS[NUM_SHAPES-1] = {&wshape_chebyshev3, &wshape_chebyshev5};

#define NUM_WAVEFORMS (NUM_SHAPES * NUM_TABLES)
const char* TABLE_NAMES[NUM_WAVEFORMS] = {
  "Squ", "Sin", "Saw", "Tri", "Chu", "Aah", "Noi",
  "Sq3", "Si3", "Sa3", "Tr3", "Ch3", "Aa3", "Noi3",
  "Sq5", "Si5", "Sa5", "Tr5", "Ch5", "Aa5", "Noi5"
};

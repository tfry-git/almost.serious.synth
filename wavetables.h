#ifndef WAVETABLES_H
#define WAVETABLES_H

/* The waveforms to use. Note that for the time being, we support exactly two distinct table sizes (TABLE_SIZE_A, TABLE_SIZE_B).
Lower table sizes, help to keep the sketch size small, but are not as pure (which may not even be a bad thing)
*/
#include <tables/sin2048_int8.h>
#include <tables/saw2048_int8.h>
#include <tables/triangle2048_int8.h>
#include <tables/square_no_alias_2048_int8.h>
#include <tables/whitenoise8192_int8.h>
#include <tables/chum9_int8.h>  // TODO: replace with something better, does not loop well.
#include "aah8192_int8.h"
#include "git8192_int8.h"
#define NUM_TABLES 7
#define TABLE_SIZE_A 2048
#define TABLE_SIZE_B 8192
#define IS_NOISE_TABLE(x) (x / NUM_TABLES >= 6)

#include <WaveShaper.h>
#include <Oscil.h>
#include <tables/waveshape_chebyshev_3rd_256_int8.h>
#include <tables/waveshape_chebyshev_5th_256_int8.h>
#include <tables/waveshape_sigmoid_int8.h>
#define NUM_SHAPES 4
#define NUM_WAVEFORMS (NUM_SHAPES * NUM_TABLES)

#if defined (DEFINE_NOW)
WaveShaper<char> wshape_chebyshev3 (CHEBYSHEV_3RD_256_DATA);
WaveShaper<char> wshape_chebyshev5 (CHEBYSHEV_5TH_256_DATA);
WaveShaper<char> wshape_sigmoid (WAVESHAPE_SIGMOID_DATA);
WaveShaper<char>* WAVE_SHAPERS[NUM_SHAPES-1] = {&wshape_chebyshev3, &wshape_chebyshev5, &wshape_sigmoid};
const int16_t WAVE_TABLE_SIZES[NUM_TABLES] = {TABLE_SIZE_A, TABLE_SIZE_A, TABLE_SIZE_A, TABLE_SIZE_A, TABLE_SIZE_B, TABLE_SIZE_B, TABLE_SIZE_B};
const int8_t* WAVE_TABLES[NUM_TABLES] = {SQUARE_NO_ALIAS_2048_DATA, SIN2048_DATA, SAW2048_DATA, TRIANGLE2048_DATA, GIT8192_DATA, AAH8192_DATA, WHITENOISE8192_DATA};
const int8_t FREQ_SHIFT[NUM_TABLES] = {0, 0, 0, 0, 7, 9, 0}; // Some "waveforms" are actually samples that should be played at a much lower rate for the same frequency
const char* TABLE_NAMES[NUM_WAVEFORMS] = {
  "Squ", "Sin", "Saw", "Tri", "Str", "Aah", "Noi",
  "Sq3", "Si3", "Sa3", "Tr3", "St3", "Aa3", "Noi3",
  "Sq5", "Si5", "Sa5", "Tr5", "St5", "Aa5", "Noi5",
  "Sq/", "Si/", "Sa/", "Tr/", "St/", "Aa/", "Noi/"
};
#else
extern WaveShaper<char> wshape_chebyshev3;
extern WaveShaper<char> wshape_chebyshev5;
extern WaveShaper<char> wshape_sigmoid;
extern WaveShaper<char>* WAVE_SHAPERS[NUM_SHAPES-1];
extern const int16_t WAVE_TABLE_SIZES[NUM_TABLES];
extern const int8_t* WAVE_TABLES[NUM_TABLES];
extern const int8_t FREQ_SHIFT[NUM_TABLES];
extern const char* TABLE_NAMES[NUM_WAVEFORMS];
#endif

/** Helper class to deal with the fact that not all tables have the same size
 *  (in particular, noise table is 8192 bytes, but including _all_ tables at
 *  that resolution would be prohibitive). Made so it will work -mostly- as a drop-in replacement
 *  to Oscil for the subset of the API we need.
 *
 *  Further, however, it adds simple support for wave-shaping in next(), and frequency adjustment
 *  for loopable sample based "wave forms" */
template<uint32_t UPDATE_FREQ> class FlexOscil {
public:
  Oscil<TABLE_SIZE_A, UPDATE_FREQ> oa;
  Oscil<TABLE_SIZE_B, UPDATE_FREQ> ob;
  void setFreq_Q24n8 (Q24n8 freq) {
    if (a) oa.setFreq_Q24n8 (freq >> FREQ_SHIFT[table_num]);
    else ob.setFreq_Q24n8 (freq >> FREQ_SHIFT[table_num]);
  }
  void setFreq_Q16n16 (Q16n16 freq) {
    if (a) oa.setFreq_Q16n16 (freq >> FREQ_SHIFT[table_num]);
    else ob.setFreq_Q16n16 (freq >> FREQ_SHIFT[table_num]);
  }
  void setPhase (unsigned int phase) {
    if (IS_NOISE_TABLE (table_num)) return; // Setting phase on noise makes the noise less noisy
    if (a) oa.setPhase (phase);
    else ob.setPhase (phase);
  }
  void setTableNum (const int8_t num) {
    table_num = num % NUM_TABLES;
    int8_t shape = num / NUM_TABLES;
    shaper = shape ? WAVE_SHAPERS[shape-1] : NULL;
    if (WAVE_TABLE_SIZES[table_num] == TABLE_SIZE_B) {
      a = false;
      ob.setTable (WAVE_TABLES[table_num]);
    } else {
      a = true;
      oa.setTable (WAVE_TABLES[table_num]);
    }
    phaseMod = 0;
  }
  inline int8_t next () {
    if (!shaper) {
      if (a) return (oa.phMod (phaseMod));
      else return (ob.phMod (phaseMod));
    }
    if (a) return (shaper->next (oa.phMod (phaseMod)));
    else return (shaper->next (ob.phMod (phaseMod)));
  }
  inline void setPhaseModulation (Q15n16 mod) {
    phaseMod = mod;
  }
  uint32_t tableSize () {
    if (a) return TABLE_SIZE_A;
    else return TABLE_SIZE_B;
  }
  bool a = true;
  int8_t table_num = 0;
  WaveShaper<char>* shaper = NULL;
  Q15n16 phaseMod = 0;
};

#endif


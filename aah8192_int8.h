#ifndef AAH8192_H_
#define AAH8192_H_
 
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <avr/pgmspace.h>

/* Choir aah taken from larger sample at https://freewavesamples.com, re-sampled 
* to be centered at 256Hz.
*
* You can set sample rate to 64, or divide frequency by 256 for use as wave table.
*
* Not quite perfect, you'll still hear a (very) slight click on each iteration of the loop.
*/

#define AAH8192_NUM_CELLS 8192
#define AAH8192_SAMPLERATE 16384
 
const int8_t __attribute__((section(".progmem.data"))) AAH8192_DATA [] = {2, 1,
2, 3, -1, -1, -5, -8, -11, -13, -10, -9, -12, -14, -14, -4, 1, -3, -11, -5, 11,
19, 21, 20, 18, 20, 15, 6, -3, -8, -7, -8, -8, -5, 4, 8, 5, 6, 6, 2, -6, -14,
-21, -28, -32, -32, -26, -18, -17, -12, -2, 3, 14, 28, 32, 34, 30, 25, 16, 15,
12, 2, -10, -21, -17, -8, -4, -4, -6, -2, 3, -2, -4, -9, -11, -5, -8, -11, -13,
-12, -5, -4, -7, -7, 3, 16, 24, 21, 19, 22, 20, 11, 1, -3, -8, -16, -17, -11, 1,
8, 6, 6, 11, 12, 8, -3, -15, -23, -28, -31, -31, -26, -19, -16, -13, -6, 11, 27,
30, 30, 27, 26, 20, 14, 7, 0, -4, -13, -18, -16, -10, -3, 1, -1, -4, -5, -2, -1,
-9, -10, -6, -8, -11, -12, -6, -3, -5, -8, 0, 12, 21, 22, 18, 26, 26, 23, 16, 8,
1, -11, -22, -19, -9, -4, -3, -1, 7, 14, 15, 5, -5, -15, -23, -31, -36, -34,
-28, -21, -18, -12, 4, 22, 30, 32, 33, 33, 25, 16, 11, 1, -10, -18, -21, -20,
-18, -13, -6, 1, 2, -1, -2, 2, 2, -4, -2, -4, -8, -10, -9, -4, -6, -10, -8, 4,
13, 20, 20, 22, 26, 26, 22, 16, 10, -2, -16, -22, -16, -12, -8, -6, 2, 12, 17,
16, 10, 2, -12, -23, -37, -38, -35, -35, -36, -28, -4, 18, 26, 30, 37, 45, 42,
24, 11, 5, -4, -19, -28, -25, -24, -23, -15, -3, 5, 4, 2, 6, 10, 4, -3, -4, -8,
-11, -14, -15, -10, -2, 1, 4, 13, 23, 30, 27, 26, 24, 18, 17, 13, 3, -12, -23,
-21, -16, -12, -8, -1, 12, 18, 18, 19, 18, 4, -14, -36, -44, -40, -41, -38, -35,
-19, 9, 28, 35, 39, 43, 42, 28, 12, 2, -8, -20, -32, -32, -26, -25, -21, -12, 1,
7, 9, 11, 13, 12, 7, 4, -2, -10, -14, -16, -14, -11, -4, 1, 7, 14, 23, 27, 26,
26, 20, 15, 9, 1, -6, -16, -19, -15, -8, -2, 2, 11, 20, 24, 19, 13, 5, -6, -28,
-52, -54, -46, -43, -36, -23, 1, 25, 38, 50, 54, 49, 36, 18, 6, -6, -20, -36,
-45, -38, -31, -28, -23, -10, 6, 15, 18, 18, 19, 18, 12, 3, -6, -16, -19, -18,
-10, -3, 2, 7, 16, 23, 25, 24, 20, 15, 10, 7, -3, -9, -11, -12, -10, -7, 2, 9,
9, 9, 16, 22, 17, 1, -12, -22, -37, -48, -50, -47, -37, -23, -4, 18, 34, 47, 58,
59, 47, 23, 4, -9, -23, -34, -49, -52, -44, -30, -14, -6, 2, 14, 27, 32, 29, 24,
15, 2, -5, -11, -17, -19, -17, -9, 3, 10, 9, 16, 22, 18, 11, 5, 5, 7, -1, -9,
-6, -3, 0, -2, 4, 11, 11, 10, 6, 6, 8, -1, -16, -26, -35, -44, -50, -45, -34,
-22, -5, 12, 32, 47, 55, 58, 54, 35, 12, -5, -21, -33, -43, -52, -51, -41, -25,
-13, -4, 7, 20, 32, 36, 35, 28, 16, 6, 1, -5, -14, -19, -15, -5, 4, 3, 6, 16,
16, 10, 8, 4, 2, 2, -3, -5, -6, 1, 9, 11, 13, 10, 9, 5, 1, -5, -12, -19, -29,
-35, -40, -40, -36, -30, -17, -2, 11, 26, 44, 54, 55, 47, 31, 13, -2, -18, -34,
-45, -55, -52, -39, -29, -19, -9, 8, 21, 25, 32, 36, 33, 25, 15, 9, 5, -5, -12,
-13, -9, -5, 0, 2, 6, 8, 7, 4, 2, 0, 0, -2, -3, 0, 1, 9, 14, 16, 16, 9, 1, -2,
-6, -15, -25, -33, -35, -35, -36, -33, -26, -14, 2, 14, 27, 45, 57, 54, 48, 35,
15, -4, -21, -31, -42, -56, -58, -47, -29, -19, -16, -3, 15, 22, 21, 24, 28, 31,
23, 13, 15, 13, 7, 3, 1, 2, 3, 0, -4, -2, -4, -8, -10, -8, -7, -3, 0, 2, 6, 12,
19, 24, 25, 15, 1, -6, -11, -18, -30, -42, -42, -35, -30, -27, -16, -3, 8, 19,
31, 46, 53, 47, 35, 27, 15, -2, -22, -37, -39, -43, -44, -43, -35, -20, -13, -6,
4, 12, 14, 17, 21, 25, 25, 18, 20, 24, 21, 14, 8, 0, -2, -2, -13, -15, -18, -17,
-16, -12, -7, -1, 6, 10, 16, 18, 21, 24, 21, 14, 2, -12, -21, -24, -34, -45,
-50, -41, -29, -17, -10, 1, 16, 25, 36, 47, 53, 49, 35, 24, 13, -1, -15, -32,
-42, -44, -42, -37, -35, -26, -15, -8, -1, 3, 7, 13, 18, 21, 23, 24, 26, 29, 32,
25, 20, 11, 1, -1, -14, -23, -25, -28, -27, -26, -19, -6, 3, 12, 18, 24, 29, 31,
27, 15, 2, -8, -17, -25, -37, -46, -48, -41, -27, -14, -5, 0, 14, 26, 37, 43,
42, 45, 39, 25, 14, 1, -15, -28, -38, -43, -43, -40, -35, -29, -20, -12, -5, 1,
0, 5, 14, 20, 21, 21, 28, 33, 40, 35, 30, 22, 9, 3, -8, -22, -32, -38, -40, -34,
-31, -18, -4, 6, 19, 27, 33, 32, 30, 21, 3, -9, -16, -25, -38, -53, -54, -43,
-27, -15, -5, 6, 22, 38, 44, 49, 50, 43, 34, 21, 9, -2, -16, -30, -38, -40, -39,
-37, -35, -28, -22, -13, -5, -3, -2, -1, 9, 19, 21, 17, 20, 29, 39, 41, 34, 26,
18, 13, 4, -7, -22, -35, -40, -39, -35, -30, -18, -6, 8, 25, 34, 36, 32, 26, 13,
-2, -13, -25, -35, -45, -49, -43, -30, -15, -5, 7, 20, 35, 47, 47, 49, 47, 37,
24, 8, -3, -10, -24, -34, -36, -37, -35, -33, -28, -26, -18, -10, -8, -2, 0, 7,
18, 21, 19, 22, 27, 30, 30, 30, 24, 19, 15, 9, 2, -7, -21, -31, -34, -34, -33,
-30, -23, -8, 11, 25, 30, 30, 26, 18, 9, -3, -19, -35, -42, -44, -44, -37, -23,
-9, 7, 20, 30, 44, 52, 54, 51, 43, 29, 8, -8, -14, -21, -34, -40, -40, -34, -31,
-29, -26, -20, -9, -9, -4, 3, 11, 19, 17, 15, 20, 25, 23, 17, 18, 22, 19, 18,
11, 9, 6, -6, -19, -29, -32, -38, -40, -32, -19, -3, 12, 24, 30, 28, 26, 20, 4,
-15, -33, -43, -43, -43, -40, -29, -11, 7, 21, 32, 39, 47, 56, 57, 44, 27, 10,
-7, -13, -20, -29, -37, -39, -33, -24, -18, -19, -19, -11, -6, -5, -2, 5, 14,
12, 11, 16, 21, 21, 13, 10, 17, 22, 20, 14, 13, 15, 9, -5, -19, -27, -31, -38,
-36, -28, -16, 1, 13, 23, 25, 23, 17, 10, -5, -25, -39, -44, -41, -38, -30, -16,
2, 21, 30, 32, 41, 56, 59, 51, 32, 15, 5, -6, -15, -28, -36, -41, -38, -28, -19,
-15, -16, -12, -5, 3, 3, -1, 4, 8, 8, 12, 14, 13, 8, 10, 14, 19, 24, 18, 19, 24,
22, 7, -9, -22, -29, -36, -44, -40, -29, -15, 2, 16, 23, 28, 23, 15, 9, -10,
-30, -45, -49, -40, -31, -22, -5, 13, 28, 39, 47, 56, 59, 51, 39, 21, 6, -4,
-18, -30, -39, -43, -38, -28, -15, -9, -8, -5, 3, 9, 5, -5, -3, 2, 0, 1, -3, -1,
4, 8, 14, 16, 26, 26, 25, 32, 31, 19, -4, -20, -27, -36, -45, -47, -36, -22, -9,
6, 20, 28, 28, 20, 14, 2, -20, -40, -52, -52, -41, -29, -15, 2, 18, 39, 55, 64,
65, 59, 45, 31, 16, -4, -18, -37, -47, -46, -39, -28, -19, -9, -1, 12, 13, 15,
12, 1, -5, -10, -11, -13, -15, -16, -7, 3, 10, 17, 24, 33, 35, 38, 38, 26, 6,
-14, -22, -34, -45, -50, -48, -32, -18, -6, 8, 24, 33, 28, 20, 12, -5, -27, -45,
-52, -48, -41, -32, -13, 8, 29, 49, 65, 71, 65, 57, 41, 27, 6, -16, -33, -49,
-47, -43, -33, -21, -9, 4, 18, 24, 22, 20, 10, -1, -13, -21, -26, -27, -27, -21,
-9, 2, 15, 25, 34, 43, 44, 40, 32, 18, -1, -17, -30, -43, -49, -52, -47, -32,
-15, 2, 18, 33, 35, 29, 19, 5, -14, -35, -49, -55, -55, -46, -29, -4, 20, 44,
66, 74, 73, 66, 52, 35, 15, -8, -30, -49, -51, -46, -36, -26, -9, 12, 21, 27,
30, 31, 21, 7, -11, -26, -34, -34, -33, -29, -16, -7, 9, 27, 38, 42, 41, 36, 33,
20, 2, -13, -27, -36, -46, -46, -43, -36, -20, -1, 14, 28, 35, 33, 24, 9, -5,
-23, -41, -56, -63, -56, -40, -19, 5, 30, 57, 70, 73, 71, 61, 44, 23, 5, -17,
-39, -49, -46, -40, -27, -11, 4, 14, 22, 32, 33, 27, 13, -7, -27, -38, -40, -37,
-36, -26, -10, 8, 27, 37, 43, 42, 36, 28, 21, 1, -16, -26, -33, -36, -40, -38,
-34, -20, -2, 11, 19, 28, 33, 29, 17, 1, -14, -33, -49, -61, -60, -50, -34, -12,
14, 43, 63, 67, 67, 67, 60, 42, 18, -6, -27, -40, -46, -41, -34, -24, -9, 8, 20,
28, 34, 31, 22, 5, -16, -33, -39, -39, -35, -28, -17, -1, 18, 35, 42, 39, 32,
24, 15, 6, -13, -27, -35, -32, -28, -29, -26, -15, 2, 13, 20, 23, 28, 29, 20, 5,
-11, -27, -42, -54, -60, -58, -42, -23, -1, 27, 50, 64, 64, 66, 70, 59, 35, 10,
-10, -22, -37, -49, -46, -34, -20, -8, 3, 15, 26, 32, 30, 17, -3, -21, -33, -39,
-38, -32, -26, -12, 7, 29, 42, 44, 34, 25, 19, 8, -9, -31, -40, -39, -35, -32,
-26, -10, 8, 20, 24, 30, 32, 30, 23, 9, -7, -22, -36, -51, -59, -62, -51, -31,
-13, 11, 35, 55, 65, 67, 72, 64, 46, 24, 3, -11, -26, -39, -46, -35, -22, -15,
-7, 2, 17, 26, 26, 17, 4, -12, -30, -38, -36, -30, -27, -16, 4, 25, 39, 45, 41,
29, 17, 4, -10, -28, -47, -49, -45, -39, -23, -6, 9, 22, 32, 42, 45, 35, 26, 12,
-5, -21, -35, -48, -56, -55, -51, -37, -20, -2, 17, 38, 56, 65, 66, 61, 53, 42,
19, -2, -16, -25, -33, -38, -30, -24, -15, -3, 4, 15, 21, 18, 7, -6, -20, -30,
-31, -28, -21, -13, -2, 17, 31, 39, 39, 27, 13, 1, -12, -25, -40, -48, -46, -44,
-32, -12, 8, 22, 29, 41, 50, 49, 35, 17, 1, -16, -30, -44, -53, -52, -48, -40,
-25, -10, 10, 29, 40, 50, 60, 63, 55, 44, 30, 13, -2, -17, -27, -30, -31, -27,
-25, -11, 0, 5, 10, 9, 12, 4, -12, -23, -25, -17, -12, -11, -6, 8, 25, 29, 29,
18, 3, -3, -13, -24, -31, -36, -38, -33, -21, -4, 9, 17, 24, 36, 43, 40, 30, 17,
4, -12, -28, -40, -50, -51, -44, -32, -18, -7, 8, 23, 35, 45, 49, 49, 46, 44,
36, 17, -1, -11, -17, -17, -23, -32, -29, -20, -6, 2, 4, 2, 2, 4, -2, -11, -22,
-19, -8, -4, -2, 0, 10, 18, 16, 8, -7, -11, -15, -24, -26, -22, -16, -13, -7, 4,
18, 23, 19, 16, 22, 25, 18, 5, -6, -13, -22, -30, -37, -39, -33, -24, -11, -3,
5, 19, 25, 29, 34, 39, 41, 36, 27, 19, 14, 5, -7, -14, -14, -21, -25, -19, -15,
-2, 5, 3, 4, 4, 1, -10, -16, -15, -8, -9, -14, -11, -2, 7, 4, -2, -5, -8, -10,
-16, -17, -8, 3, 7, 9, 13, 22, 28, 21, 11, 3, 3, -4, -12, -18, -25, -26, -25,
-23, -24, -21, -12, -2, 8, 15, 18, 21, 26, 31, 33, 29, 25, 22, 13, 4, 1, 3, 0,
-7, -11, -15, -8, -4, -4, 0, -2, -3, -5, -10, -11, -15, -14, -6, -4, -8, -9, -5,
-1, -2, -6, -11, -15, -16, -10, -4, 1, 13, 22, 23, 24, 22, 25, 19, 1, -15, -24,
-25, -31, -34, -33, -23, -10, -5, -5, -3, 8, 17, 18, 12, 14, 18, 19, 17, 16, 21,
21, 14, 5, 4, 2, 2, 3, 0, 1, -4, -6, 2, 3, 2, -4, -15, -15, -13, -15, -15, -15,
-7, -2, -3, -7, -8, -4, -7, -13, -12, -9, -7, -7, 2, 12, 26, 34, 26, 25, 24, 21,
12, -7, -22, -33, -37, -42, -45, -38, -27, -13, -2, 3, 9, 21, 25, 28, 23, 19,
15, 8, 9, 9, 13, 13, 10, 5, 5, 4, 6, 6, 1, 1, 1, 3, 5, 5, 0, -4, -12, -16, -21,
-22, -19, -17, -10, -3, -4, -6, -6, -8, -7, -10, -9, -6, -3, 0, 10, 21, 26, 34,
34, 29, 21, 10, 1, -11, -25, -34, -37, -39, -44, -41, -32, -17, -5, -2, 1, 15,
25, 30, 31, 20, 16, 12, 11, 9, 6, 8, 7, 0, 2, 7, 9, 9, 5, 8, 8, 8, 5, 4, -2, -9,
-18, -25, -28, -31, -27, -22, -11, 2, 6, 2, 2, 3, 5, -3, -13, -10, -3, -2, -2,
10, 26, 42, 44, 32, 23, 17, 4, -11, -27, -38, -43, -45, -46, -40, -29, -15, -9,
-4, 6, 18, 26, 24, 26, 26, 22, 16, 9, 9, 8, 4, 2, -1, 2, 7, 13, 16, 12, 9, 8, 9,
4, 0, -6, -16, -22, -25, -28, -33, -29, -19, -10, -1, 8, 10, 14, 13, 6, 1, -5,
-6, -5, -8, -8, 8, 24, 33, 38, 37, 29, 20, 3, -13, -24, -36, -45, -50, -45, -36,
-26, -18, -12, 0, 6, 10, 17, 21, 25, 22, 19, 17, 14, 11, 7, 3, 0, -3, -2, 2, 10,
14, 14, 14, 14, 14, 9, -1, -12, -22, -29, -35, -40, -37, -33, -23, -12, 1, 17,
22, 18, 19, 17, 9, 2, -6, -6, -7, -12, -4, 11, 24, 33, 36, 30, 22, 15, -1, -18,
-34, -45, -48, -45, -37, -27, -13, -7, 2, 9, 13, 14, 11, 19, 21, 16, 13, 10, 17,
16, 8, 1, -3, 1, -2, 1, 5, 8, 15, 17, 19, 19, 10, -6, -17, -25, -31, -40, -44,
-41, -32, -16, -4, 10, 22, 22, 19, 18, 12, 9, 2, -9, -6, -4, -2, 7, 21, 29, 32,
28, 21, 18, 1, -19, -33, -41, -45, -47, -38, -30, -18, -7, 0, 8, 11, 13, 12, 13,
20, 17, 12, 11, 16, 21, 11, 4, -1, 1, -2, -10, -7, -2, 9, 16, 17, 19, 21, 13, 0,
-14, -29, -40, -46, -49, -44, -33, -17, 1, 16, 21, 18, 18, 19, 15, 8, -2, -5, 3,
6, 11, 19, 25, 28, 26, 17, 13, 1, -18, -31, -39, -38, -41, -40, -31, -20, -8,
-1, 5, 9, 8, 10, 12, 17, 21, 19, 19, 18, 19, 17, 10, -1, -8, -9, -15, -14, -10,
-4, 9, 19, 25, 28, 28, 17, 4, -8, -25, -40, -51, -57, -49, -34, -16, 1, 6, 8,
16, 27, 27, 15, 9, 9, 15, 21, 19, 22, 27, 28, 21, 9, 4, -3, -19, -34, -42, -39,
-39, -41, -35, -22, -10, -1, 3, 8, 11, 15, 19, 17, 23, 24, 21, 19, 19, 15, 5,
-6, -13, -15, -21, -26, -22, -11, 1, 13, 22, 34, 44, 40, 25, 11, -4, -23, -40,
-59, -64, -56, -46, -29, -14, -7, 3, 15, 27, 33, 29, 25, 26, 34, 40, 37, 31, 21,
14, 7, -5, -16, -32, -43, -45, -44, -37, -31, -23, -13, -5, 5, 9, 7, 3, 3, 10,
10, 12, 23, 30, 29, 22, 18, 14, 5, -9, -21, -30, -37, -33, -21, -11, 2, 18, 33,
45, 53, 47, 31, 15, -7, -31, -51, -67, -67, -65, -58, -39, -24, -9, 8, 24, 39,
44, 44, 45, 45, 46, 47, 38, 22, 9, -1, -13, -29, -43, -50, -48, -46, -43, -28,
-9, 4, 7, 10, 14, 14, 7, 1, 0, -4, -1, 12, 26, 27, 22, 21, 21, 14, -1, -14, -26,
-38, -41, -34, -23, -9, 9, 26, 43, 58, 61, 50, 31, 10, -14, -40, -62, -74, -74,
-74, -65, -45, -23, 2, 22, 38, 48, 56, 60, 58, 52, 47, 39, 22, 9, -2, -21, -40,
-49, -47, -44, -45, -44, -29, -6, 15, 18, 12, 10, 12, 10, -4, -11, -11, -6, 4,
16, 30, 35, 30, 24, 17, 8, -5, -19, -37, -45, -40, -29, -12, 6, 22, 36, 55, 67,
62, 39, 13, -7, -29, -50, -69, -84, -86, -71, -46, -26, -8, 13, 35, 49, 59, 64,
60, 51, 43, 39, 29, 12, -6, -18, -32, -45, -49, -47, -42, -41, -31, -14, 6, 20,
15, 9, 11, 11, 2, -10, -17, -14, -2, 11, 21, 29, 34, 36, 29, 12, 0, -7, -26,
-41, -46, -40, -23, 0, 17, 30, 45, 59, 61, 44, 20, -1, -21, -46, -67, -77, -77,
-67, -48, -28, -7, 14, 34, 46, 52, 56, 56, 48, 39, 30, 23, 17, 2, -13, -24, -37,
-40, -40, -40, -37, -33, -23, -9, 10, 16, 8, 4, 9, 8, -5, -16, -17, -5, 5, 12,
21, 31, 33, 29, 19, 7, 3, -10, -29, -38, -35, -22, -5, 12, 21, 33, 45, 49, 38,
22, 5, -18, -36, -56, -66, -68, -63, -45, -27, -12, 6, 27, 42, 49, 52, 54, 50,
39, 29, 21, 15, 7, -9, -18, -30, -40, -38, -37, -33, -31, -30, -20, -3, 11, 8,
0, 4, 9, 2, -6, -11, -5, 8, 11, 13, 22, 29, 27, 17, 5, 0, -3, -16, -31, -33,
-21, -6, 7, 18, 27, 37, 41, 36, 22, 4, -18, -36, -51, -62, -60, -54, -44, -27,
-7, 11, 23, 32, 41, 48, 49, 42, 35, 28, 29, 20, 10, 1, -9, -18, -32, -38, -40,
-39, -39, -36, -28, -19, -2, 8, 10, 10, 11, 6, 1, 1, 5, 9, 8, 11, 20, 28, 26,
16, 7, 1, 0, -6, -20, -29, -22, -10, 1, 9, 17, 25, 29, 25, 19, 9, -12, -31, -44,
-51, -51, -49, -41, -27, -6, 11, 19, 27, 35, 41, 43, 39, 33, 29, 27, 21, 11, 7,
-1, -11, -22, -35, -42, -42, -39, -40, -40, -33, -16, 1, 9, 12, 15, 15, 14, 13,
13, 14, 11, 12, 12, 18, 21, 14, 6, 2, 0, -4, -11, -18, -19, -11, -3, 4, 11, 16,
19, 19, 13, 4, -6, -24, -37, -43, -39, -38, -35, -25, -11, 6, 17, 24, 28, 34,
40, 41, 39, 31, 27, 26, 17, 7, 2, -8, -22, -34, -41, -45, -43, -45, -44, -40,
-28, -10, 3, 10, 17, 24, 24, 24, 24, 27, 22, 15, 9, 8, 8, 2, 1, -1, 1, 2, -2,
-6, -6, -4, 0, -1, 0, 5, 3, 0, -2, -7, -8, -19, -30, -32, -29, -22, -18, -12,
-4, 7, 13, 15, 22, 31, 31, 28, 29, 30, 29, 23, 16, 11, 7, -2, -18, -33, -42,
-49, -51, -48, -45, -42, -36, -20, -4, 9, 19, 27, 30, 34, 34, 31, 29, 21, 12, 1,
-2, -6, -4, -2, -2, 4, 0, 1, 3, 4, 0, -6, -5, 1, -2, -11, -13, -14, -14, -19,
-24, -28, -28, -20, -13, -2, 7, 12, 14, 17, 27, 33, 28, 21, 22, 22, 23, 19, 14,
9, 5, 0, -8, -20, -32, -41, -50, -47, -42, -42, -40, -34, -17, 0, 11, 22, 33,
38, 41, 41, 40, 32, 18, 2, -5, -8, -8, -4, -2, 4, 7, 7, 13, 13, 5, -6, -8, -4,
-10, -21, -29, -27, -19, -19, -25, -29, -26, -11, 0, 8, 15, 20, 26, 26, 28, 30,
26, 16, 10, 7, 12, 15, 10, 5, 5, 7, 1, -10, -23, -35, -43, -45, -43, -44, -45,
-40, -26, -5, 9, 20, 31, 38, 44, 48, 43, 32, 20, 6, -6, -11, -12, -12, -7, 3,
10, 14, 15, 16, 13, 5, -2, -8, -19, -31, -34, -36, -32, -27, -25, -23, -23, -13,
4, 14, 18, 23, 29, 30, 29, 28, 25, 16, 9, 6, 1, 4, 9, 8, 4, 1, 1, -2, -14, -30,
-41, -44, -44, -45, -45, -41, -30, -11, 6, 18, 26, 33, 42, 48, 42, 31, 21, 14,
4, -5, -12, -13, -8, 4, 13, 12, 12, 16, 18, 14, 4, -5, -17, -32, -38, -38, -37,
-35, -32, -26, -16, -6, 6, 18, 24, 31, 35, 35, 31, 25, 21, 19, 9, 0, -8, -6, 4,
3, 0, 1, 6, 5, -6, -19, -28, -37, -45, -48, -44, -40, -32, -18, -4, 13, 28, 33,
38, 40, 37, 30, 21, 15, 8, -1, -11, -14, -6, 5, 14, 13, 10, 14, 18, 18, 9, -6,
-16, -30, -36, -41, -45, -42, -35, -28, -19, -6, 6, 24, 31, 30, 34, 36, 32, 22,
16, 17, 11, -2, -9, -10, -1, 1, -4, -2, 3, 7, -1, -10, -21, -29, -36, -48, -46,
-39, -33, -25, -12, 6, 25, 33, 33, 34, 31, 26, 18, 12, 5, -3, -8, -8, 1, 9, 15,
15, 16, 22, 21, 13, 3, -6, -11, -24, -38, -44, -45, -42, -37, -28, -18, -4, 7,
20, 35, 38, 37, 35, 29, 23, 14, 8, 2, -3, -8, -10, -5, 1, 0, 2, 8, 12, 6, -6,
-15, -26, -33, -41, -44, -39, -37, -26, -9, 5, 17, 27, 32, 32, 26, 22, 13, 5,
-2, -7, -8, -7, 4, 13, 18, 22, 23, 25, 21, 14, 6, -7, -13, -23, -38, -45, -43,
-41, -38, -30, -15, -2, 7, 19, 30, 37, 31, 28, 23, 16, 7, 1, -4, -5, 0, -1, -2,
-1, 2, 5, 10, 10, 6, -3, -8, -18, -28, -32, -42, -42, -38, -28, -10, 3, 13, 20,
25, 28, 25, 17, 5, -3, -6, -11, -9, -6, 4, 14, 23, 35, 33, 29, 24, 18, 13, -3,
-16, -25, -34, -42, -45, -44, -37, -25, -12, -2, 9, 24, 30, 31, 29, 27, 19, 6,
-1, -3, -2, -6, -5, 3, 8, 5, 5, 7, 13, 14, 3, -10, -15, -16, -27, -32, -34, -34,
-30, -23, -6, 12, 18, 16, 17, 18, 19, 9, -4, -12, -13, -10, -9, -6, 3, 15, 23,
34, 38, 30, 26, 23, 21, 6, -10, -21, -31, -36, -40, -42, -41, -34, -16, 1, 14,
20, 27, 29, 29, 26, 12, -1, -7, -8, -11, -12, -8, 3, 10, 10, 11, 16, 22, 18, 9,
-7, -15, -19, -30, -39, -39, -35, -29, -17, -1, 13, 20, 20, 21, 19, 11, 1, -12,
-18, -19, -18, -15, -6, 5, 15, 22, 30, 37, 35, 27, 23, 22, 14, 2, -16, -33, -34,
-32, -38, -43, -39, -17, 3, 13, 19, 27, 31, 25, 21, 9, -6, -15, -18, -16, -11,
-5, 2, 11, 16, 20, 22, 22, 16, 8, -1, -15, -23, -31, -38, -41, -37, -30, -20,
-2, 17, 26, 27, 25, 19, 9, -3, -14, -20, -26, -28, -19, -5, 11, 18, 22, 28, 34,
34, 25, 19, 20, 16, 5, -5, -17, -26, -32, -35, -34, -32, -25, -11, 6, 20, 31,
31, 25, 16, 6, -5, -15, -23, -23, -18, -7, 7, 15, 21, 29, 31, 29, 20, 9, 0, -13,
-27, -38, -38, -36, -37, -35, -23, -8, 10, 22, 27, 26, 19, 11, -1, -11, -19,
-24, -23, -15, -1, 9, 16, 20, 24, 30, 28, 18, 14, 13, 14, 9, 2, -5, -17, -26,
-33, -34, -34, -29, -15, 0, 17, 32, 38, 33, 19, 7, -5, -21, -32, -35, -28, -21,
-8, 12, 29, 39, 37, 35, 33, 23, 5, -14, -29, -35, -38, -40, -41, -36, -24, -13,
-1, 15, 22, 23, 19, 14, 6, -9, -19, -20, -15, -6, 4, 10, 19, 26, 28, 26, 19, 11,
6, 2, -3, -5, -8, -8, -10, -16, -22, -24, -20, -19, -8, 4, 16, 30, 37, 36, 23,
5, -9, -21, -33, -43, -44, -35, -16, 4, 21, 38, 49, 51, 44, 32, 16, 4, -15, -34,
-44, -44, -40, -38, -32, -19, -6, 5, 15, 19, 22, 15, 7, -3, -9, -8, -11, -7, 2,
14, 25, 26, 24, 27, 27, 16, 3, -6, -9, -13, -19, -23, -25, -21, -18, -15, -13,
-10, 0, 13, 25, 36, 41, 35, 24, 9, -6, -23, -44, -52, -49, -39, -27, -11, 12,
38, 53, 54, 49, 39, 27, 9, -13, -32, -41, -46, -47, -42, -32, -20, -6, 4, 12,
17, 18, 15, 4, -5, -11, -9, -7, -8, 0, 13, 24, 25, 31, 34, 33, 21, 9, 2, -10,
-22, -35, -39, -39, -35, -26, -18, -10, 1, 13, 26, 39, 48, 48, 39, 26, 11, -9,
-27, -43, -58, -57, -51, -37, -15, 5, 25, 43, 57, 57, 47, 32, 16, 1, -18, -36,
-47, -46, -39, -31, -24, -14, 4, 12, 16, 19, 19, 10, -6, -15, -14, -5, -6, -6,
5, 22, 33, 38, 43, 38, 32, 18, 5, -8, -24, -41, -51, -52, -47, -34, -25, -18, 1,
21, 41, 53, 58, 60, 55, 35, 8, -11, -31, -47, -63, -70, -60, -44, -23, -2, 20,
37, 51, 56, 51, 38, 21, 4, -14, -23, -32, -40, -39, -35, -25, -15, 1, 10, 11,
15, 13, 10, -1, -12, -17, -18, -15, -10, 3, 14, 26, 38, 49, 49, 37, 26, 11, -1,
-18, -39, -53, -62, -60, -48, -38, -28, -6, 16, 38, 58, 73, 76, 67, 48, 23, 0,
-23, -51, -69, -73, -71, -55, -35, -12, 12, 31, 46, 54, 50, 37, 24, 9, -6, -19,
-25, -30, -31, -25, -22, -15, 0, 14, 11, 9, 13, 15, 10, -9, -17, -28, -28, -19,
-13, -1, 11, 33, 50, 57, 52, 43, 29, 6, -12, -32, -49, -63, -70, -62, -49, -31,
-9, 14, 37, 57, 74, 82, 78, 57, 30, 3, -17, -40, -71, -86, -81, -63, -42, -21,
5, 25, 43, 56, 56, 42, 22, 8, -8, -18, -23, -25, -29, -23, -10, -2, 10, 17, 14,
10, 11, 11, 7, -6, -20, -31, -36, -30, -24, -15, -1, 20, 43, 56, 59, 58, 47, 22,
-2, -22, -44, -60, -72, -79, -68, -41, -15, 12, 34, 57, 78, 88, 83, 65, 38, 7,
-20, -40, -64, -86, -89, -75, -51, -26, -2, 19, 41, 59, 59, 46, 27, 12, -5, -18,
-26, -28, -27, -21, -4, 14, 19, 25, 26, 19, 11, 2, -2, -9, -24, -39, -47, -39,
-27, -22, -4, 17, 40, 60, 67, 65, 56, 40, 17, -12, -45, -64, -76, -83, -79, -58,
-31, -1, 32, 59, 79, 88, 84, 71, 51, 21, -14, -42, -58, -74, -82, -76, -61, -40,
-16, 13, 42, 54, 50, 45, 40, 28, 4, -16, -26, -30, -29, -22, -8, 11, 21, 26, 33,
30, 23, 5, -9, -16, -24, -36, -54, -57, -42, -26, -8, 12, 32, 55, 67, 68, 64,
55, 29, -5, -34, -53, -71, -89, -91, -73, -41, -11, 24, 51, 73, 87, 87, 80, 60,
32, -5, -40, -62, -74, -80, -78, -65, -45, -23, 6, 34, 52, 50, 42, 39, 31, 15,
-8, -24, -28, -25, -19, -9, 5, 21, 30, 33, 30, 25, 15, -1, -17, -28, -40, -53,
-59, -53, -38, -16, 11, 32, 48, 63, 69, 67, 59, 35, 3, -24, -46, -67, -84, -87,
-74, -48, -18, 14, 45, 69, 85, 88, 80, 65, 35, -3, -37, -58, -74, -86, -84, -62,
-38, -18, 4, 29, 51, 53, 43, 35, 24, 10, -7, -19, -22, -19, -11, 0, 15, 24, 31,
32, 27, 22, 13, -1, -16, -27, -42, -52, -55, -55, -44, -25, -4, 23, 45, 60, 63,
60, 57, 44, 17, -18, -43, -64, -74, -75, -71, -51, -21, 11, 39, 63, 76, 80, 77,
61, 36, 5, -31, -59, -73, -79, -81, -69, -50, -23, 6, 29, 49, 56, 54, 43, 28,
10, -9, -23, -24, -17, -10, 1, 15, 28, 38, 35, 26, 16, 11, -2, -19, -30, -41,
-45, -48, -50, -41, -25, -9, 9, 37, 51, 59, 58, 51, 46, 25, -8, -37, -58, -72,
-72, -67, -49, -19, 9, 32, 55, 75, 81, 68, 46, 28, 5, -29, -56, -69, -74, -74,
-65, -44, -22, 0, 25, 48, 63, 62, 51, 30, 10, -7, -23, -29, -27, -16, -2, 15,
32, 42, 43, 37, 22, 6, -8, -22, -33, -46, -50, -41, -38, -35, -29, -10, 7, 22,
38, 47, 52, 43, 38, 30, 12, -19, -45, -60, -64, -58, -46, -22, 1, 25, 46, 62,
68, 59, 40, 18, -6, -32, -52, -66, -70, -65, -52, -33, -15, 1, 22, 43, 57, 61,
54, 36, 15, -1, -17, -27, -26, -20, -8, 0, 17, 33, 42, 44, 28, 13, -2, -18, -33,
-42, -47, -46, -41, -34, -21, -9, 6, 15, 32, 47, 45, 38, 33, 31, 17, -10, -34,
-47, -52, -51, -42, -26, -10, 12, 34, 46, 54, 53, 37, 15, -7, -31, -46, -55,
-59, -55, -46, -27, -7, 15, 24, 32, 44, 49, 46, 33, 12, -5, -13, -18, -15, -13,
-10, 0, 13, 26, 28, 32, 28, 18, 7, -13, -26, -33, -40, -41, -39, -37, -28, -11,
5, 12, 20, 37, 42, 39, 34, 27, 18, 2, -19, -35, -44, -43, -40, -31, -15, 1, 18,
30, 37, 43, 35, 17, -3, -22, -32, -37, -46, -49, -42, -23, -8, 5, 23, 30, 35,
38, 38, 28, 7, -11, -13, -7, -2, 0, -1, 6, 12, 22, 22, 18, 17, 11, 2, -11, -20,
-24, -29, -31, -30, -30, -26, -19, -7, 7, 17, 27, 35, 33, 37, 35, 21, 7, -7,
-23, -34, -38, -36, -35, -27, -10, 5, 21, 27, 27, 27, 17, 7, -9, -25, -31, -39,
-36, -31, -24, -11, -1, 14, 27, 32, 30, 28, 23, 7, -9, -15, -5, 2, 6, 8, 12, 18,
23, 23, 14, 6, 1, -5, -16, -24, -24, -17, -10, -12, -13, -15, -17, -10, 0, 3, 5,
10, 17, 30, 33, 25, 12, 4, -3, -10, -23, -33, -31, -29, -18, -2, 7, 11, 13, 18,
16, 5, -6, -20, -25, -27, -30, -28, -24, -13, -3, 8, 18, 28, 30, 23, 17, 7, -2,
-8, -10, -3, 5, 13, 16, 18, 24, 22, 14, 6, -4, -10, -20, -30, -21, -8, 1, 3, -2,
-1, 0, -5, -11, -11, -10, -6, 0, 10, 20, 29, 27, 22, 20, 11, 0, -17, -25, -28,
-29, -20, -11, -5, 0, 5, 8, 7, 0, -13, -20, -19, -17, -20, -22, -20, -8, 6, 16,
22, 24, 17, 14, 13, 5, -2, -7, -5, 2, 8, 13, 17, 21, 21, 15, 13, 3, -13, -22,
-25, -20, -9, 1, 5, 4, 6, 5, -2, -9, -14, -15, -16, -12, -2, 11, 24, 34, 37, 37,
28, 13, -2, -20, -29, -40, -38, -31, -24, -14, -5, 4, 8, 8, 2, -5, -15, -18,
-20, -24, -22, -13, -2, 8, 13, 23, 28, 18, 12, 7, 4, 4, -2, -4, 2, 8, 14, 11,
12, 12, 9, 6, -5, -15, -19, -19, -10, 4, 8, 7, 5, 6, 4, -4, -13, -18, -13, -13,
-11, 2, 20, 38, 43, 40, 38, 30, 13, -15, -35, -46, -47, -44, -41, -30, -15, 2,
12, 17, 15, 7, -3, -13, -22, -31, -31, -21, -12, -2, 9, 19, 31, 31, 27, 24, 16,
11, 5, -5, -6, -7, -1, 0, -4, -1, -2, 2, 1, -4, -8, -6, 2, 13, 15, 11, 9, 8, 3,
-4, -8, -16, -20, -20, -21, -10, 11, 31, 42, 42, 41, 39, 28, 2, -24, -40, -48,
-48, -48, -44, -32, -12, 6, 13, 12, 7, -2, -13, -20, -27, -32, -29, -19, -4, 11,
23, 33, 40, 38, 34, 29, 20, 5, -9, -16, -17, -15, -13, -14, -13, -8, 0, 7, 7, 4,
5, 14, 23, 25, 19, 10, 5, 0, -9, -14, -18, -23, -21, -18, -9, 9, 32, 46, 50, 47,
46, 40, 12, -20, -42, -52, -57, -62, -63, -50, -23, 1, 12, 13, 15, 10, 1, -11,
-21, -23, -25, -22, -11, 9, 27, 32, 32, 36, 39, 34, 22, 1, -14, -20, -23, -23,
-21, -19, -19, -9, 1, 12, 17, 11, 10, 24, 33, 30, 19, 8, 4, -3, -13, -19, -20,
-20, -18, -12, 4, 17, 30, 44, 52, 52, 41, 31, 8, -20, -44, -59, -63, -68, -70,
-58, -29, 0, 16, 17, 16, 17, 12, 0, -15, -24, -25, -20, -11, 0, 14, 26, 31, 36,
33, 24, 18, 4, -9, -19, -29, -27, -22, -21, -17, -8, 4, 15, 20, 16, 13, 23, 31,
26, 14, 3, 0, -4, -13, -18, -15, -10, -4, -1, 11, 27, 36, 41, 42, 39, 34, 21,
-2, -26, -46, -56, -60, -65, -68, -56, -32, -5, 14, 21, 21, 22, 20, 9, -5, -17,
-20, -23, -19, -6, 6, 16, 21, 26, 27, 24, 19, 6, -7, -14, -18, -22, -21, -17,
-14, -10, -2, 9, 16, 18, 13, 20, 30, 28, 14, 2, 3, -2, -12, -17, -17, -7, 3, 4,
8, 22, 38, 44, 40, 33, 27, 20, 0, -28, -47, -56, -58, -62, -62, -52, -34, -8, 6,
15, 22, 22, 21, 17, 5, -10, -15, -18, -19, -9, -1, 8, 19, 23, 23, 19, 16, 8, 0,
-7, -18, -27, -21, -15, -15, -15, -10, 4, 14, 19, 17, 22, 28, 24, 18, 8, -1,
-10, -17, -15, -11, -6, 1, 7, 18, 30, 43, 44, 37, 30, 22, 13, -6, -29, -48, -59,
-59, -57, -56, -49, -34, -8, 10, 19, 21, 21, 23, 21, 13, -3, -13, -15, -13, -9,
-5, -1, 6, 13, 18, 19, 14, 6, 3, -1, -10, -21, -23, -18, -14, -11, -8, 0, 10,
14, 17, 20, 27, 26, 17, 7, 0, -8, -15, -15, -13, -4, 3, 14, 26, 35, 44, 46, 40,
27, 11, -3, -14, -28, -43, -55, -57, -50, -47, -38, -27, -10, 5, 11, 18, 21, 19,
15, 9, -1, -10, -13, -9, -7, -5, -2, 3, 12, 14, 11, 11, 11, 5, -3, -9, -13, -16,
-16, -17, -17, -9, -5, -1, 4, 12, 17, 21, 22, 19, 14, 5, -5, -9, -6, -7, -5, -4,
8, 28, 40, 44, 41, 36, 27, 15, -6, -22, -35, -43, -45, -46, -44, -41, -29, -19,
-7, 4, 7, 7, 11, 16, 13, 6, -3, -8, -8, -5, -1, -1, 0, 1, 11, 14, 9, 9, 8, 4, 2,
-4, -12, -14, -15, -17, -19, -17, -12, -10, -5, 3, 11, 19, 22, 22, 18, 11, 1,
-2, -2, -6, -6, -3, 9, 24, 35, 41, 43, 39, 32, 20, 0, -22, -35, -41, -45, -47,
-44, -33, -25, -16, -5, 4, 3, 2, 8, 16, 12, 0, -3, -5, -3, -3, -4, 0, 6, 8, 10,
12, 11, 10, 4, -1, -3, -10, -20, -20, -16, -16, -19, -19, -13, -6, -2, 2, 8, 17,
19, 19, 18, 10, -2, -5, -2, -5, -6, -4, 8, 22, 31, 38, 44, 41, 33, 22, 5, -13,
-28, -37, -43, -44, -43, -38, -27, -13, -2, 1, -1, 1, 9, 13, 10, 0, -3, -2, -3,
-2, -3, 1, 10, 11, 10, 11, 11, 11, 2, -7, -12, -16, -21, -20, -16, -14, -11,
-10, -7, -5, -2, 1, 4, 11, 10, 5, 1, 3, 3, -1, -2, -1, 4, 10, 16, 22, 30, 41,
43, 35, 29, 21, 4, -11, -23, -31, -34, -38, -41, -39, -26, -15, -8, -4, -5, -2,
4, 12, 11, 4, 2, 5, 7, 4, 2, 3, 8, 9, 8, 8, 6, 3, -3, -13, -18, -18, -24, -29,
-27, -14, -1, 2, 2, 0, 3, 7, 8, 5, -2, -6, -9, -9, -5, -3, 1, 6, 12, 17, 22, 28,
34, 42, 44, 34, 22, 15, 9, -4, -18, -28, -31, -32, -36, -36, -30, -21, -14, -11,
-9, -4, 8, 15, 12, 7, 7, 15, 15, 7, 5, 4, 5, 4, 5, 2, -2, -8, -12, -15, -20,
-23, -27, -25, -18, -9, 6, 14, 15, 11, 6, 3, 0, -3, -9, -18, -26, -22, -16, -5,
6, 13, 21, 24, 33, 44, 48, 47, 43, 33, 21, 14, 2, -12, -24, -31, -30, -27, -32,
-34, -25, -17, -11, -11, -12, -6, 5, 11, 9, 13, 15, 21, 23, 14, 10, 5, 4, 3, -7,
-14, -18, -20, -22, -23, -24, -24, -20, -15, -2, 5, 12, 23, 23, 17, 4, -7, -9,
-14, -23, -30, -37, -29, -19, -9, 8, 19, 31, 33, 37, 49, 54, 53, 44, 33, 22, 11,
-1, -12, -25, -33, -35, -34, -29, -28, -26, -19, -11, -3, -1, -3, 0, 5, 10, 13,
16, 22, 24, 15, 10, 6, 4, 3, -6, -17, -25, -27, -28, -32, -31, -28, -18, -9, 3,
13, 18, 31, 32, 21, 6, -7, -14, -20, -31, -42, -43, -32, -18, -8, 12, 27, 38,
41, 43, 50, 53, 49, 37, 28, 18, 8, -2, -13, -23, -31, -33, -31, -27, -25, -20,
-9, 0, 1, -2, -4, -1, 0, -2, 1, 7, 17, 20, 15, 11, 10, 7, 5, -1, -13, -22, -23,
-27, -36, -35, -28, -21, -14, -6, 7, 20, 33, 35, 27, 15, 0, -12, -23, -34, -45,
-45, -36, -24, -10, 8, 22, 36, 42, 45, 50, 49, 44, 35, 28, 17, 7, -4, -11, -16,
-22, -25, -25, -25, -19, -12, -8, -4, -2, -2, -5, -12, -14, -10, -7, 0, 9, 20,
25, 22, 17, 14, 14, 9, -8, -25, -32, -35, -35, -34, -32, -23, -13, -7, 3, 18,
34, 36, 33, 23, 10, 1, -16, -29, -39, -45, -40, -28, -15, 1, 13, 29, 41, 46, 49,
44, 39, 33, 25, 14, 5, -5, -8, -11, -14, -12, -11, -14, -14, -8, -4, 3, 0, -7,
-16, -19, -19, -23, -22, -17, 0, 18, 25, 27, 27, 28, 28, 19, 3, -14, -31, -41,
-43, -39, -31, -25, -21, -12, 3, 16, 30, 34, 30, 26, 18, 9, -8, -27, -39, -45,
-38, -28, -21, -7, 12, 29, 41, 47, 49, 45, 38, 28, 19, 8, -4, -12, -13, -9, -9,
-10, -8, -4, 0, 3, 1, -1, -3, -7, -11, -22, -31, -34, -31, -20, -10, 7, 17, 26,
33, 38, 41, 27, 9, -7, -24, -37, -46, -47, -37, -30, -19, -6, 4, 13, 26, 34, 31,
26, 19, 8, -8, -25, -34, -35, -35, -32, -19, -2, 14, 27, 35, 46, 48, 43, 35, 21,
10, -1, -12, -17, -16, -13, -12, -8, -3, 0, 8, 14, 11, 2, -2, -3, -12, -23, -30,
-35, -38, -32, -16, 1, 10, 21, 30, 38, 43, 38, 22, 3, -16, -31, -42, -48, -42,
-35, -21, -4, 6, 15, 26, 34, 33, 28, 17, 2, -16, -27, -33, -36, -34, -30, -17,
1, 21, 37, 42, 42, 45, 44, 34, 14, -4, -14, -18, -21, -23, -23, -13, -1, 8, 12,
18, 24, 19, 6, -3, -8, -20, -28, -32, -37, -37, -32, -21, -3, 9, 19, 29, 38, 42,
35, 23, 6, -9, -27, -40, -45, -42, -33, -20, -6, 3, 15, 28, 37, 35, 24, 12, -1,
-12, -23, -32, -33, -32, -27, -13, 3, 21, 33, 41, 44, 43, 37, 21, 6, -6, -19,
-28, -29, -23, -16, -7, 4, 15, 24, 25, 27, 22, 6, -10, -24, -31, -35, -36, -38,
-38, -27, -13, 5, 20, 27, 33, 36, 35, 30, 22, 5, -17, -32, -36, -39, -40, -35,
-22, -3, 11, 20, 26, 36, 38, 28, 16, -1, -14, -21, -30, -32, -32, -28, -14, -2,
17, 33, 42, 46, 45, 41, 23, 4, -10, -23, -34, -38, -34, -20, -8, 4, 20, 33, 38,
34, 27, 10, -7, -26, -43, -47, -46, -42, -38, -28, -7, 15, 34, 41, 41, 40, 34,
26, 12, -4, -22, -40, -46, -42, -33, -25, -22, -8, 15, 30, 35, 32, 30, 25, 21,
2, -16, -20, -25, -22, -24, -28, -17, -6, 7, 29, 36, 40, 41, 34, 23, 7, -12,
-28, -37, -41, -34, -21, -9, 4, 21, 39, 47, 42, 28, 12, -7, -26, -43, -51, -54,
-52, -43, -30, -8, 17, 38, 48, 50, 46, 41, 34, 13, -9, -29, -43, -46, -47, -45,
-33, -20, -3, 15, 28, 41, 39, 34, 24, 13, 2, -13, -19, -27, -26, -20, -14, -10,
-4, 9, 26, 38, 38, 37, 32, 20, -3, -22, -33, -40, -46, -43, -29, -10, 9, 27, 46,
52, 51, 41, 20, -1, -25, -44, -56, -60, -57, -46, -31, -12, 15, 38, 53, 55, 49,
40, 33, 17, -8, -31, -44, -44, -42, -39, -32, -21, -5, 12, 25, 32, 29, 25, 20,
10, 0, -10, -16, -19, -14, -8, -3, 0, 4, 15, 25, 30, 33, 28, 20, 9, -8, -22,
-37, -42, -43, -40, -30, -16, 8, 29, 47, 54, 51, 46, 24, 0, -23, -45, -56, -62,
-60, -49, -31, -9, 15, 32, 47, 54, 49, 37, 27, 15, -4, -21, -39, -46, -41, -37,
-31, -17, -8, 2, 14, 24, 27, 24, 16, 5, -5, -9, -8, -10, -12, -8, 3, 14, 15, 16,
25, 30, 31, 27, 13, -3, -16, -29, -36, -37, -42, -43, -29, -13, 8, 28, 39, 49,
51, 48, 31, 4, -20, -40, -49, -53, -55, -49, -31, -8, 15, 33, 43, 48, 44, 34,
23, 13, -3, -18, -33, -40, -38, -33, -28, -16, -5, 2, 12, 21, 22, 20, 14, 6, -6,
-15, -7, -5, -3, -2, 0, 16, 26, 30, 28, 28, 27, 22, 12, -7, -20, -29, -38, -42,
-42, -35, -23, -12, 2, 23, 36, 40, 39, 36, 29, 9, -13, -29, -39, -42, -41, -37,
-29, -10, 14, 32, 43, 42, 35, 25, 16, 5, -7, -26, -40, -38, -34, -28, -21, -11,
-3, 3, 12, 18, 21, 18, 10, 2, -6, -13, -7, -3, -1, 0, 4, 16, 22, 29, 32, 30, 27,
18, 10, -1, -16, -31, -39, -45, -45, -40, -27, -9, 6, 17, 29, 38, 39, 32, 19, 4,
-10, -20, -32, -32, -29, -26, -19, -5, 14, 29, 38, 36, 24, 16, 9, -3, -18, -31,
-36, -34, -30, -26, -14, -5, 3, 8, 12, 13, 11, 6, 1, -1, -5, -7, -4, -1, 8, 14,
17, 21, 23, 30, 30, 21, 15, 12, 10, -2, -14, -24, -31, -37, -40, -36, -26, -17,
0, 13, 22, 30, 29, 27, 16, 3, -7, -14, -21, -24, -20, -18, -10, 3, 15, 25, 29,
28, 23, 14, 2, -10, -20, -32, -38, -36, -30, -23, -18, -7, 5, 14, 18, 14, 7, 5,
1, -2, -8, -9, 0, 4, 11, 15, 22, 34, 32, 25, 22, 18, 10, 0, -7, -8, -13, -20,
-28, -26, -24, -26, -21, -15, -3, 8, 12, 16, 21, 22, 17, 3, -9, -13, -15, -19,
-21, -17, -8, 6, 18, 25, 30, 28, 20, 10, -3, -16, -27, -38, -44, -43, -33, -18,
-7, -1, 8, 17, 24, 19, 4, -1, -5, -3, -6, -10, -2, 5, 18, 29, 35, 37, 35, 29,
21, 10, -1, -11, -20, -21, -19, -16, -17, -12, -8, -8, -5, -4, 1, 6, 3, -2, 3,
9, 7, -2, -11, -8, -5, -6, -10, -10, -2, 10, 22, 24, 24, 24, 17, 7, -8, -21,
-31, -41, -46, -45, -34, -19, -7, -1, 10, 23, 26, 21, 9, 3, -2, -7, -9, -9, 0,
4, 10, 21, 34, 44, 38, 28, 22, 13, 2, -9, -22, -27, -22, -20, -20, -11, -3, 0,
-2, -4, 5, 12, 4, -7, -7, -4, -5, -10, -14, -7, -2, 1, 2, 4, 7, 10, 18, 20, 19,
16, 5, -5, -11, -19, -30, -41, -47, -45, -37, -21, -6, 5, 15, 23, 28, 26, 16, 5,
-3, -9, -14, -14, -8, 0, 11, 22, 32, 41, 40, 34, 27, 16, 7, -3, -16, -27, -27,
-22, -20, -15, -10, -4, 0, -1, 3, 13, 14, 3, -5, -7, -3, -7, -16, -15, -11, -3,
6, 11, 15, 19, 23, 25, 17, 9, -1, -14, -25, -30, -34, -39, -42, -43, -32, -15,
1, 12, 19, 22, 28, 25, 16, 7, -6, -11, -18, -20, -16, -6, 8, 18, 27, 37, 44, 40,
29, 21, 14, 4, -8, -19, -26, -26, -24, -20, -15, -10, -6, 1, 4, 11, 19, 12, 3,
0, -2, -4, -11, -16, -14, -12, -2, 8, 21, 27, 27, 27, 20, 10, -5, -25, -41, -42,
-43, -45, -44, -39, -22, -2, 11, 19, 27, 28, 27, 20, 12, 4, -9, -18, -24, -24,
-18, -12, 0, 13, 22, 32, 40, 43, 34, 23, 16, 12, 7, -8, -19, -20, -18, -16, -19,
-16, -14, -11, -3, 3, 8, 11, 10, 8, 6, 6, 2, -6, -8, -8, -3, 0, 8, 17, 26, 30,
21, 11, -1, -16, -35, -50, -57, -57, -51, -41, -23, -1, 16, 28, 35, 39, 38, 24,
11, -3, -14, -19, -27, -31, -32, -23, -4, 10, 16, 28, 37, 41, 38, 32, 28, 16, 7,
0, -7, -11, -14, -16, -17, -16, -15, -17, -14, -6, -1, 4, 8, 13, 15, 12, 8, 4,
3, -3, -7, -7, 2, 12, 19, 23, 19, 15, 7, -11, -27, -43, -56, -59, -55, -47, -28,
-4, 16, 27, 37, 42, 39, 28, 11, -2, -13, -19, -29, -36, -35, -28, -12, 3, 13,
20, 30, 39, 40, 35, 30, 19, 11, 5, 1, -5, -13, -17, -19, -14, -9, -9, -13, -13,
-9, -2, 6, 10, 9, 10, 14, 14, 10, 4, 1, 1, 2, 8, 15, 18, 14, 9, 5, -9, -26, -40,
-51, -55, -55, -50, -34, -11, 11, 25, 37, 44, 42, 34, 20, 3, -11, -21, -32, -39,
-41, -32, -16, -2, 11, 22, 30, 34, 33, 28, 25, 18, 11, 5, 0, -1, -1, -4, -6, -4,
-2, -7, -15, -16, -13, -5, -4, -3, 3, 10, 14, 13, 9, 11, 13, 9, 6, 10, 18, 20,
16, 9, 3, -12, -31, -44, -50, -55, -56, -51, -33, -6, 14, 22, 33, 40, 39, 32,
17, 4, -9, -22, -31, -35, -35, -30, -21, -6, 8, 20, 26, 29, 27, 23, 23, 18, 15,
10, 4, 0, 1, 2, 6, 6, -1, -9, -17, -17, -16, -12, -7, -5, 0, 5, 13, 20, 17, 11,
9, 10, 13, 17, 17, 15, 14, 13, 6, -10, -29, -47, -57, -60, -60, -53, -36, -11,
11, 27, 37, 40, 39, 32, 19, 2, -11, -23, -30, -35, -35, -25, -19, -12, -2, 14,
26, 26, 19, 16, 19, 16, 15, 14, 11, 10, 9, 10, 11, 9, -1, -10, -19, -19, -16,
-13, -9, -7, 1, 10, 18, 17, 13, 7, 3, 2, 7, 16, 19, 19, 20, 24, 17, 2, -19, -38,
-55, -68, -70, -63, -43, -19, 4, 22, 37, 45, 48, 34, 16, 1, -11, -20, -31, -35,
-37, -25, -14, -9, 0, 11, 20, 22, 18, 14, 13, 14, 14, 11, 10, 12, 13, 13, 14,
14, 8, -2, -12, -17, -15, -13, -9, -4, -2, 2, 10, 17, 14, 3, -5, -6, 3, 13, 20,
24, 24, 25, 27, 17, -4, -28, -53, -70, -69, -63, -53, -36, -13, 13, 32, 41, 44,
38, 26, 13, -6, -20, -24, -29, -34, -29, -19, -12, -6, 1, 12, 17, 11, 7, 11, 15,
19, 14, 13, 14, 17, 20, 16, 15, 7, -4, -11, -12, -15, -17, -13, -6, 2, 2, 6, 11,
10, 4, 1, 0, 1, 9, 19, 26, 27, 27, 29, 24, 5, -17, -41, -62, -70, -68, -56, -43,
-27, -4, 16, 32, 45, 43, 36, 22, 5, -8, -15, -21, -29, -29, -27, -19, -12, -12,
-5, 7, 12, 11, 14, 15, 25, 25, 23, 22, 17, 18, 11, 10, 6, -3, -12, -16, -16,
-15, -13, -7, -1, 1, 7, 14, 14, 10, 9, 8, 9, 10, 13, 17, 19, 22, 19, 14, 4, -12,
-30, -46, -57, -61, -53, -42, -29, -10, 4, 15, 32, 38, 34, 22, 7, 0, -5, -12,
-23, -27, -27, -22, -20, -21, -12, -2, 4, 8, 14, 18, 25, 28, 29, 25, 17, 15, 9,
5, -1, -8, -16, -18, -16, -13, -9, -5, 4, 7, 10, 13, 19, 20, 16, 10, 7, 12, 12,
13, 8, 10, 13, 7, -4, -16, -22, -33, -45, -47, -44, -34, -26, -16, -3, 6, 19,
26, 27, 23, 11, 1, -1, -3, -13, -18, -20, -20, -23, -20, -15, -8, -4, 0, 7, 18,
28, 29, 28, 20, 15, 13, 6, 4, 1, -6, -15, -16, -10, -4, 1, 0, 1, 5, 11, 13, 15,
18, 20, 15, 11, 15, 17, 13, 7, 4, 5, 2, -7, -18, -30, -33, -35, -37, -39, -33,
-20, -11, -3, -1, 7, 20, 25, 21, 10, 0, -1, -1, -6, -10, -14, -15, -17, -18,
-14, -9, -8, -9, -5, 5, 16, 21, 20, 14, 10, 13, 12, 10, 4, -4, -6, -2, -4, -3,
1, 4, 5, 5, 9, 9, 8, 12, 19, 17, 13, 11, 15, 18, 12, 7, 3, 0, -6, -17, -27, -32,
-33, -33, -34, -27, -21, -12, -5, -3, 0};

#endif /* AAH8192_H_ */


/*
 *
 * random.c - the random number generator
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "random.h"

// ---------------------------------------------------------------------------
// variables
// ---------------------------------------------------------------------------

static long long randomSeed;

// ---------------------------------------------------------------------------
// set the seed
// ---------------------------------------------------------------------------

void randomSetSeed(long long seed) {

  randomSeed = seed;
}

// ---------------------------------------------------------------------------
// yank the next 32bit pseudo random value
// ---------------------------------------------------------------------------

int  randomNextInt(void) {

  randomSeed = (randomSeed * 0x5DEECE66DLL + 0xB) & ((1LL << 48) - 1);
  return (int)(randomSeed >> (48 - 32));
}


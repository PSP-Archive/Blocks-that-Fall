
/*
 *
 * save.c - the save file manager
 *
 */

#include <pspkernel.h>
#include <pspctrl.h>
#include <pspumd.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "main.h"
#include "save.h"
#include "game.h"

// ---------------------------------------------------------------------------
// variables
// ---------------------------------------------------------------------------

unsigned char saveData[8];

// ---------------------------------------------------------------------------
// load a save file
// ---------------------------------------------------------------------------

void saveLoad(void) {

	FILE *f;
	int i;

	f = fopen("save.bin", "rb");
	if (f == NULL)
		return;

	fread(saveData, 1, 8, f);
	fclose(f);

	// it the save file ok?
	if (saveData[0] == 'B' &&
			saveData[1] == 't' &&
			saveData[2] == 'F' &&
			saveData[3] == SAVE_VERSION_NUMBER) {
		// yes! parse the file
		i = 4;

		gameHiScore = (saveData[i+0] << 24) | (saveData[i+1] << 16) | (saveData[i+2] << 8) | saveData[i+3];
		i += 4;
	}
}

// ---------------------------------------------------------------------------
// save a save file
// ---------------------------------------------------------------------------

void saveSave(void) {

	int i, j;
	FILE *f;

	// write the save data
	i = 0;

	saveData[i++] = 'B';
	saveData[i++] = 't';
	saveData[i++] = 'F';
	saveData[i++] = SAVE_VERSION_NUMBER;

	j = gameHiScore;
	saveData[i++] = j >> 24;
	saveData[i++] = j >> 16;
	saveData[i++] = j >> 8;
	saveData[i++] = j;

	f = fopen("save.bin", "wb");
	if (f == NULL)
		return;

	fwrite(saveData, 1, 8, f);
	fclose(f);
}

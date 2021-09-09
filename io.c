
/*
 *
 * io.c - i/o manager
 *
 */

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspctrl.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "io.h"

// ---------------------------------------------------------------------------
// variables
// ---------------------------------------------------------------------------

// the current key bits
int ioKeyBits;

// ---------------------------------------------------------------------------
// init the i/o
// ---------------------------------------------------------------------------

void ioInit(void) {

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

	// reset the keys
	ioKeyBits = 0;
}

// ---------------------------------------------------------------------------
// read the keys
// ---------------------------------------------------------------------------

void ioUpdate(void) {

	SceCtrlData pad;

	sceCtrlReadBufferPositive(&pad, 1);

	// parse the bits
	ioKeyBits = 0;

	if (pad.Buttons & PSP_CTRL_SELECT)
		ioKeyBits |= IO_KEY_BIT_SELECT;
	if (pad.Buttons & PSP_CTRL_START)
		ioKeyBits |= IO_KEY_BIT_START;
	if (pad.Buttons & PSP_CTRL_UP)
		ioKeyBits |= IO_KEY_BIT_UP;
	if (pad.Buttons & PSP_CTRL_DOWN)
		ioKeyBits |= IO_KEY_BIT_DOWN;
	if (pad.Buttons & PSP_CTRL_LEFT)
		ioKeyBits |= IO_KEY_BIT_LEFT;
	if (pad.Buttons & PSP_CTRL_RIGHT)
		ioKeyBits |= IO_KEY_BIT_RIGHT;
	if (pad.Buttons & PSP_CTRL_LTRIGGER)
		ioKeyBits |= IO_KEY_BIT_L;
	if (pad.Buttons & PSP_CTRL_RTRIGGER)
		ioKeyBits |= IO_KEY_BIT_R;
	if (pad.Buttons & PSP_CTRL_TRIANGLE)
		ioKeyBits |= IO_KEY_BIT_Y;
	if (pad.Buttons & PSP_CTRL_CIRCLE)
		ioKeyBits |= IO_KEY_BIT_B;
	if (pad.Buttons & PSP_CTRL_SQUARE)
		ioKeyBits |= IO_KEY_BIT_X;
	if (pad.Buttons & PSP_CTRL_CROSS)
		ioKeyBits |= IO_KEY_BIT_A;
}

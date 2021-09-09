
/*
 *
 * main.c - inits, and the main loop
 *
 */

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspgu.h>
#include <pspgum.h>
#include <psprtc.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "common/callbacks.h"
#include "common/vram.h"

#include "main.h"
#include "gfx.h"
#include "game.h"
#include "sound.h"
#include "io.h"
#include "random.h"
#include "save.h"
#include "particle.h"

// ---------------------------------------------------------------------------
// PSP info
// ---------------------------------------------------------------------------

PSP_MODULE_INFO("Blocks that Fall v1.5", 0, 1, 5);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

// ---------------------------------------------------------------------------
// the main loop
// ---------------------------------------------------------------------------

int main(int argc, char* argv[]) {

	unsigned int frameCounter = 0;
	float deltaTime, tickResolution;
	u64 tickLast, tickCurrent;

	// set the callbacks
	setupCallbacks();

	// init the game
	gfxInit();
	soundInit();
	ioInit();
	gameInit();
	particlesInit();
	randomSetSeed(10);
	saveLoad();

	// init the clock
	tickResolution = sceRtcGetTickResolution() / 1000.0f;
	sceRtcGetCurrentTick(&tickLast);

	// main loop
	while (running()) {
		// calculate deltaTime
		sceRtcGetCurrentTick(&tickCurrent);
		deltaTime = (tickCurrent - tickLast) / tickResolution;
		tickLast = tickCurrent;

		if (deltaTime < 0.001f)
			deltaTime = 0.001f;

		// update the key bits
		ioUpdate();

		// advance the game
		gameLive(deltaTime);
		gfxGameBackgroundLive(deltaTime);
		particlesLive(deltaTime);

		// draw the background
		gfxGameBackgroundDraw();

		// draw the blocks
		gfxGameBlocksDraw();

		// swap buffers
		gfxSwapBuffers();

		frameCounter++;
	}

	sceGuTerm();
	sceKernelExitGame();

	return 0;
}

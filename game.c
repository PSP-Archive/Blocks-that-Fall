
/*
 *
 * game.c - the game logic
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "main.h"
#include "game.h"
#include "io.h"
#include "random.h"
#include "save.h"
#include "gfx.h"
#include "sound.h"
#include "particle.h"

// ---------------------------------------------------------------------------
// variables
// ---------------------------------------------------------------------------

// the tile map
unsigned char tileMap[TILE_MAP_DY * TILE_MAP_DX];

// score
unsigned int gameScore;

// hi score
unsigned int gameHiScore = 0;

// score adder
unsigned int gameScoreAdder;

// level
unsigned int gameLevel;

// lines
unsigned int gameLines;

// main timer
float gameTimer;

// score adder timer
float gameScoreAdderTimer;

// block drop timer
float gameBlockDropTimer;

// block move timer
float gameBlockMoveTimer;

// has the game ended?
int gameOver = YES;

// the block under control
int gameBlockX;
int gameBlockY;
int gameBlockType;
int gameBlockRotation;
int gameBlockDropLines;

// the next block
int gameBlockNext = GAME_BLOCK_TYPE_L;

// the old keys
unsigned int keysOld;

// is the game paused?
int gamePaused = NO;

// ---------------------------------------------------------------------------
// draw all score values
// ---------------------------------------------------------------------------

void gameScoresDraw(void) {

	// draw hi score
	gameValueDraw(gameHiScore, 14, 2, 10000);

	// draw score
	gameValueDraw(gameScore, 14, 5, 10000);

	// draw level
	gameValueDraw(gameLevel, 14, 8, 10000);

	// draw lines
	gameValueDraw(gameLines, 14, 11, 10000);
}

// ---------------------------------------------------------------------------
// init the game
// ---------------------------------------------------------------------------

void gameInit(void) {

	int i, j;

	// reseed the random number generator
	randomSetSeed((int)gameTimer);

	// clear the tile map
	for (i = 0; i < TILE_MAP_DY * TILE_MAP_DX; i++)
		tileMap[i] = TILE_TYPE_NONE;

	// draw the walls around the playfield
	for (i = 0; i < TILE_MAP_DY; i++) {
		tileMap[i*TILE_MAP_DX +  1] = TILE_TYPE_WALL;
		tileMap[i*TILE_MAP_DX + 12] = TILE_TYPE_WALL;
	}

	// fill the borders
	for (i = 0; i < TILE_MAP_DY; i++) {
		for (j = 0; j < TILE_MAP_DX; j++) {
			if (j >= 1 && j <= 12)
				continue;
			if (i == 0 || j == 0 || i == TILE_MAP_DY-1 || j == TILE_MAP_DX-1)
				tileMap[i*TILE_MAP_DX + j] = TILE_TYPE_BLACK_BORDER;
			else
				tileMap[i*TILE_MAP_DX + j] = TILE_TYPE_BLACK;
		}
	}

	// draw HI
	tileMap[1*TILE_MAP_DX + 17] = TILE_TYPE_LETTER_H;
	tileMap[1*TILE_MAP_DX + 18] = TILE_TYPE_LETTER_I;

	// draw SCORE
	tileMap[4*TILE_MAP_DX + 14] = TILE_TYPE_LETTER_S;
	tileMap[4*TILE_MAP_DX + 15] = TILE_TYPE_LETTER_C;
	tileMap[4*TILE_MAP_DX + 16] = TILE_TYPE_LETTER_O;
	tileMap[4*TILE_MAP_DX + 17] = TILE_TYPE_LETTER_R;
	tileMap[4*TILE_MAP_DX + 18] = TILE_TYPE_LETTER_E;

	// draw LEVEL
	tileMap[7*TILE_MAP_DX + 14] = TILE_TYPE_LETTER_L;
	tileMap[7*TILE_MAP_DX + 15] = TILE_TYPE_LETTER_E;
	tileMap[7*TILE_MAP_DX + 16] = TILE_TYPE_LETTER_V;
	tileMap[7*TILE_MAP_DX + 17] = TILE_TYPE_LETTER_E;
	tileMap[7*TILE_MAP_DX + 18] = TILE_TYPE_LETTER_L;

	// draw LINES
	tileMap[10*TILE_MAP_DX + 14] = TILE_TYPE_LETTER_L;
	tileMap[10*TILE_MAP_DX + 15] = TILE_TYPE_LETTER_I;
	tileMap[10*TILE_MAP_DX + 16] = TILE_TYPE_LETTER_N;
	tileMap[10*TILE_MAP_DX + 17] = TILE_TYPE_LETTER_E;
	tileMap[10*TILE_MAP_DX + 18] = TILE_TYPE_LETTER_S;

	// reset score, level, and timer
	gameScore = 0;
	gameScoreAdder = 0;
	gameScoreAdderTimer = 0.0f;
	gameLevel = 1;
	gameLines = 0;
	gameTimer = 0.0f;
	gameBlockDropTimer = 0.0f;
	gameBlockMoveTimer = 0.0f;

	// reset the block under control
	gameBlockGiveNext();

	// draw all scores
	gameScoresDraw();
}

// ---------------------------------------------------------------------------
// intro (i.e. randomize) the next block
// ---------------------------------------------------------------------------

void gameBlockGiveNext(void) {

	gameBlockX = 6;
	gameBlockY = 0;
	gameBlockType = gameBlockNext;
	gameBlockRotation = 0;
	gameBlockDropLines = 0;

	// randomize the next block
	gameBlockNext = (randomNextInt() & 0x0FFFFFFF) % 7;

	// make the preview throb
	previewBlockScale = 3.0f;
}

// ---------------------------------------------------------------------------
// draw the value
// ---------------------------------------------------------------------------

int tileNumbers[10] = {
	TILE_TYPE_NUMBER_0,
	TILE_TYPE_NUMBER_1,
	TILE_TYPE_NUMBER_2,
	TILE_TYPE_NUMBER_3,
	TILE_TYPE_NUMBER_4,
	TILE_TYPE_NUMBER_5,
	TILE_TYPE_NUMBER_6,
	TILE_TYPE_NUMBER_7,
	TILE_TYPE_NUMBER_8,
	TILE_TYPE_NUMBER_9
};

void gameValueDraw(int value, int vX, int vY, int divider) {

	int v, draw = 0;

	while (1) {
		v = value / divider;
		if (v != 0 || draw == 1 || divider == 1) {
			draw = 1;
			tileMap[vY*TILE_MAP_DX + vX] = tileNumbers[v];
		}
		value -= v * divider;
		vX++;

		if (divider == 1)
			return;

		divider /= 10;
	}
}

// ---------------------------------------------------------------------------
// blocks (with rotations)
// ---------------------------------------------------------------------------

unsigned char blockMasksI[4][4*4] = {
	{
		0, 0, 0, 0,
		1, 1, 1, 1,
		0, 0, 0, 0,
		0, 0, 0, 0
	},
	{
		0, 1, 0, 0,
		0, 1, 0, 0,
		0, 1, 0, 0,
		0, 1, 0, 0
	},
	{
		0, 0, 0, 0,
		1, 1, 1, 1,
		0, 0, 0, 0,
		0, 0, 0, 0
	},
	{
		0, 1, 0, 0,
		0, 1, 0, 0,
		0, 1, 0, 0,
		0, 1, 0, 0
	}
};

unsigned char blockMasksJ[4][4*4] = {
	{
		0, 0, 0, 0,
		1, 0, 0, 0,
		1, 1, 1, 0,
		0, 0, 0, 0
	},
	{
		0, 1, 0, 0,
		0, 1, 0, 0,
		1, 1, 0, 0,
		0, 0, 0, 0
	},
	{
		0, 0, 0, 0,
		1, 1, 1, 0,
		0, 0, 1, 0,
		0, 0, 0, 0
	},
	{
		1, 1, 0, 0,
		1, 0, 0, 0,
		1, 0, 0, 0,
		0, 0, 0, 0
	}
};

unsigned char blockMasksL[4][4*4] = {
	{
		0, 0, 0, 0,
		0, 0, 1, 0,
		1, 1, 1, 0,
		0, 0, 0, 0
	},
	{
		1, 1, 0, 0,
		0, 1, 0, 0,
		0, 1, 0, 0,
		0, 0, 0, 0
	},
	{
		0, 0, 0, 0,
		1, 1, 1, 0,
		1, 0, 0, 0,
		0, 0, 0, 0
	},
	{
		1, 0, 0, 0,
		1, 0, 0, 0,
		1, 1, 0, 0,
		0, 0, 0, 0
	}
};

unsigned char blockMasksO[4][4*4] = {
	{
		0, 0, 0, 0,
		0, 1, 1, 0,
		0, 1, 1, 0,
		0, 0, 0, 0
	},
	{
		0, 0, 0, 0,
		0, 1, 1, 0,
		0, 1, 1, 0,
		0, 0, 0, 0
	},
	{
		0, 0, 0, 0,
		0, 1, 1, 0,
		0, 1, 1, 0,
		0, 0, 0, 0
	},
	{
		0, 0, 0, 0,
		0, 1, 1, 0,
		0, 1, 1, 0,
		0, 0, 0, 0
	}
};

unsigned char blockMasksS[4][4*4] = {
	{
		0, 0, 0, 0,
		0, 1, 1, 0,
		1, 1, 0, 0,
		0, 0, 0, 0
	},
	{
		1, 0, 0, 0,
		1, 1, 0, 0,
		0, 1, 0, 0,
		0, 0, 0, 0
	},
	{
		0, 0, 0, 0,
		0, 1, 1, 0,
		1, 1, 0, 0,
		0, 0, 0, 0
	},
	{
		1, 0, 0, 0,
		1, 1, 0, 0,
		0, 1, 0, 0,
		0, 0, 0, 0
	}
};

unsigned char blockMasksT[4][4*4] = {
	{
		0, 0, 0, 0,
		0, 1, 0, 0,
		1, 1, 1, 0,
		0, 0, 0, 0
	},
	{
		0, 1, 0, 0,
		1, 1, 0, 0,
		0, 1, 0, 0,
		0, 0, 0, 0
	},
	{
		0, 0, 0, 0,
		1, 1, 1, 0,
		0, 1, 0, 0,
		0, 0, 0, 0
	},
	{
		0, 1, 0, 0,
		0, 1, 1, 0,
		0, 1, 0, 0,
		0, 0, 0, 0
	}
};

unsigned char blockMasksZ[4][4*4] = {
	{
		0, 0, 0, 0,
		1, 1, 0, 0,
		0, 1, 1, 0,
		0, 0, 0, 0
	},
	{
		0, 1, 0, 0,
		1, 1, 0, 0,
		1, 0, 0, 0,
		0, 0, 0, 0
	},
	{
		0, 0, 0, 0,
		1, 1, 0, 0,
		0, 1, 1, 0,
		0, 0, 0, 0
	},
	{
		0, 1, 0, 0,
		1, 1, 0, 0,
		1, 0, 0, 0,
		0, 0, 0, 0
	}
};

// each block is drawn repeating one tile
unsigned char blockTiles[] = {
	TILE_TYPE_BLOCK_4, // I
	TILE_TYPE_BLOCK_1, // J
	TILE_TYPE_BLOCK_1, // L
	TILE_TYPE_BLOCK_2, // O
	TILE_TYPE_BLOCK_3, // S
	TILE_TYPE_BLOCK_5, // T
	TILE_TYPE_BLOCK_6  // Z
};

// ---------------------------------------------------------------------------
// return the mask specifying the rotated block
// ---------------------------------------------------------------------------

unsigned char *gameBlockGetMask(int type, int rotation) {

	// TODO: replace this with a table

	if (type == GAME_BLOCK_TYPE_I)
		return blockMasksI[rotation];
	else if (type == GAME_BLOCK_TYPE_J)
		return blockMasksJ[rotation];
	else if (type == GAME_BLOCK_TYPE_L)
		return blockMasksL[rotation];
	else if (type == GAME_BLOCK_TYPE_O)
		return blockMasksO[rotation];
	else if (type == GAME_BLOCK_TYPE_S)
		return blockMasksS[rotation];
	else if (type == GAME_BLOCK_TYPE_T)
		return blockMasksT[rotation];

	return blockMasksZ[rotation];
}

// ---------------------------------------------------------------------------
// draw the block
// ---------------------------------------------------------------------------

void gameBlockDraw(int bX, int bY, int type, int rotation) {

	int tX, tY, cX, cY, bit;
	unsigned char *mask;

	mask = gameBlockGetMask(type, rotation);

	for (tY = 0; tY < 4; tY++) {
		for (tX = 0; tX < 4; tX++) {
			bit = mask[(tY << 2) + tX];
			if (bit == 0)
				continue;

			cX = bX + tX;
			cY = bY + tY;

			tileMap[cY*TILE_MAP_DX + cX] = blockTiles[type];
		}
	}
}

// ---------------------------------------------------------------------------
// remove the block from the play field
// ---------------------------------------------------------------------------

void gameBlockRemove(int bX, int bY, int type, int rotation) {

	int tX, tY, cX, cY, bit;
	unsigned char *mask;

	mask = gameBlockGetMask(type, rotation);

	for (tY = 0; tY < 4; tY++) {
		for (tX = 0; tX < 4; tX++) {
			bit = mask[(tY << 2) + tX];
			if (bit == 0)
				continue;

			cX = bX + tX;
			cY = bY + tY;

			tileMap[cY*TILE_MAP_DX + cX] = TILE_TYPE_NONE;
		}
	}
}

// ---------------------------------------------------------------------------
// test if the block can be placed on the play field
// ---------------------------------------------------------------------------

int  gameBlockTest(int bX, int bY, int type, int rotation) {

	int tX, tY, cX, cY, bit;
	unsigned char *mask;

	mask = gameBlockGetMask(type, rotation);

	for (tY = 0; tY < 4; tY++) {
		for (tX = 0; tX < 4; tX++) {
			bit = mask[(tY << 2) + tX];
			if (bit == 0)
				continue;

			cX = bX + tX;
			cY = bY + tY;

			// out of play area?
			if (cX <= 1 || cX >= 12 || cY >= 18)
				return NO;

			// overlapping other tiles?
			if (tileMap[cY*TILE_MAP_DX + cX] != TILE_TYPE_NONE)
				return NO;

			// ... no collision!
		}
	}

	return YES;
}

// ---------------------------------------------------------------------------
// remove all full lines
// ---------------------------------------------------------------------------

int gameLineBonus[] = {
	0,
	100,
	300,
	600,
	1200
};

void gameEvaluateLines(void) {

	int tY, tX, lines, i;
	float f;

	lines = 0;
	for (tY = 0; tY < 18; tY++) {
		// is the line full?
		for (tX = 2; tX < 12; tX++) {
			if (tileMap[tY*TILE_MAP_DX + tX] == TILE_TYPE_NONE)
				break;
		}

		if (tX == 12) {
			// remove the full line
			while (tY > 0) {
				for (tX = 2; tX < 12; tX++)
					tileMap[tY*TILE_MAP_DX + tX] = tileMap[(tY-1)*TILE_MAP_DX + tX];
				tY--;
			}

			lines++;
		}
	}

	gameScoreAdder += gameLineBonus[lines];

	gameLines += lines;
	gameLevel = (gameLines / 10) + 1;

	while (lines > 0) {
		// play boom!
		soundPlaySFX(SFX_REMOVE_ROW);

		// add missiles to the background
		for (i = 0; i < 5; i++) {
			f = (randomNextInt() & 7) / 3.5f;
			if ((randomNextInt() & 1) == 0)
				particleAdd(-16.0f*3.0f - f, randomNextInt() % (9*3), PARTICLE_TYPE_MISSILE);
			else
				particleAdd(randomNextInt() % (16*3), -9.0f*3.0f - f, PARTICLE_TYPE_MISSILE);
		}

		lines--;
	}
}

// ---------------------------------------------------------------------------
// advance the game
// ---------------------------------------------------------------------------

void gameLive(float deltaTime) {

	int bX, bY, bR, down, ok;
	unsigned int keys = ioKeyBits;
	float dropTime;

	// advance the main timer
	gameTimer += deltaTime;

	// manage the score adder
	gameScoreAdderTimer += deltaTime;
	if (gameScoreAdder == 0)
		gameScoreAdderTimer = 0.0f;
	else {
		while (gameScoreAdderTimer > 0.0f && gameScoreAdder > 0) {
			// increase the score by one every 5ms
			gameScoreAdderTimer -= 5.0f;
			gameScoreAdder--;
			gameScore++;
		}
	}

	// update hi score
	if (gameHiScore < gameScore)
		gameHiScore = gameScore;

	// draw all scores
	gameScoresDraw();

	// SFX & MP3 toggle
	if ((keysOld & IO_KEY_BIT_SELECT) == 0 && (keys & IO_KEY_BIT_SELECT) != 0) {
		if (soundGotMusic == NO) {
			// we only have SFX
			if (soundSFX == YES)
				soundSFXSet(NO);
			else
				soundSFXSet(YES);
		}
		else {
			if (soundSFX == YES && soundMusic == YES)
				soundMusicSet(NO);
			else if (soundSFX == YES && soundMusic == NO)
				soundSFXSet(NO);
			else if (soundSFX == NO && soundMusic == NO)
				soundMusicSet(YES);
			else
				soundSFXSet(YES);
		}
	}

	if (gameOver == YES) {
		// start a new game?
		if ((keysOld & IO_KEY_BIT_START) == 0 && (keys & IO_KEY_BIT_START) != 0) {
			gameInit();
			gameOver = NO;
		}

		keysOld = keys;

		return;
	}

	// pause / unpause?
	if ((keysOld & IO_KEY_BIT_START) == 0 && (keys & IO_KEY_BIT_START) != 0) {
		if (gamePaused == YES)
			gamePaused = NO;
		else
			gamePaused = YES;
	}

	if (gamePaused == YES) {
		keysOld = keys;

		return;
	}

	// move the block
	bX = gameBlockX;
	bY = gameBlockY;
	bR = gameBlockRotation;

	// clear the old position
	gameBlockRemove(bX, bY, gameBlockType, bR);

	// reset the drop timer?
	if ((keysOld & IO_KEY_BIT_DOWN) == 0 && (keys & IO_KEY_BIT_DOWN) != 0)
		gameBlockDropTimer = 0.0f;

	// run the move timer
	if ((keys & IO_KEY_BIT_LEFT) != 0 || (keys & IO_KEY_BIT_RIGHT) != 0)
		gameBlockMoveTimer += deltaTime;
	else
		gameBlockMoveTimer = 0.0f;

	// drop the block?
	dropTime = 1500.0f - gameLevel*250.0f;
	if (dropTime < 30.0f)
		dropTime = 30.0f;

	if ((keys & IO_KEY_BIT_DOWN) != 0)
		dropTime = 30.0f;
	else
		gameBlockDropLines = 0;

	gameBlockDropTimer += deltaTime;
	if (gameBlockDropTimer >= dropTime) {
		// yes!
		gameBlockDropTimer -= dropTime;
		down = YES;
		bY++;

		if ((keys & IO_KEY_BIT_DOWN) != 0)
			gameBlockDropLines++;
	}
	else {
		// no, use player input!
		down = NO;

		if ((keys & IO_KEY_BIT_LEFT) != 0) {
			if (gameBlockMoveTimer > 60.0f) {
				gameBlockMoveTimer -= 60.0f;
				bX--;
			}
			if ((keysOld & IO_KEY_BIT_LEFT) == 0)
				bX--;
		}

		if ((keys & IO_KEY_BIT_RIGHT) != 0) {
			if (gameBlockMoveTimer > 60.0f) {
				gameBlockMoveTimer -= 60.0f;
				bX++;
			}
			if ((keysOld & IO_KEY_BIT_RIGHT) == 0)
				bX++;
		}

		if ((keysOld & IO_KEY_BIT_A) == 0 && (keys & IO_KEY_BIT_A) != 0) {
			// flip the block
			soundPlaySFX(SFX_FLIP);

			bR = (bR + 1) & 3;
		}
	}

	// can the move be done?
	ok = gameBlockTest(bX, bY, gameBlockType, bR);

	if (ok == NO) {
		// restore the old block configuration
		bX = gameBlockX;
		bY = gameBlockY;
		bR = gameBlockRotation;
	}

	// repaint the block
	gameBlockDraw(bX, bY, gameBlockType, bR);

	// store the new configuration
	gameBlockX = bX;
	gameBlockY = bY;
	gameBlockRotation = bR;

	if (down == YES && ok == NO) {
		// add some to the score
		gameScoreAdder += gameBlockDropLines;

		// play the hit SFX
		soundPlaySFX(SFX_HIT);

		// evaluate the situation
		gameEvaluateLines();

		// the block stays here, so start a new block
		gameBlockGiveNext();

		// game over?
		if (gameBlockTest(gameBlockX, gameBlockY, gameBlockType, gameBlockRotation) == NO) {
			// yes
			gameOver = YES;

			// save the hi score?
			if (gameScore == gameHiScore)
				saveSave();
		}
	}

	// store the key bits
	keysOld = keys;
}

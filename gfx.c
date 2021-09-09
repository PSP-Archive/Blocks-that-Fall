
/*
 *
 * gfx.c - draw graphics
 *
 */

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspgu.h>
#include <pspgum.h>
#include <psppower.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "common/vram.h"

#include "main.h"
#include "gfx.h"
#include "game.h"
#include "io.h"
#include "inflate.h"
#include "particle.h"

// ---------------------------------------------------------------------------
// variables
// ---------------------------------------------------------------------------

// room for the texture
static unsigned char __attribute__((aligned(16))) textureUnpacked[64*64*4];

// display list
static unsigned int __attribute__((aligned(16))) list[262144];

// texture
extern unsigned char texture_start[];

// one 3D tile
struct vertex __attribute__((aligned(16))) tileVerticesWall[12*3] = {
	{ 0, 1, 0xff7f0000, -1, -1, 1}, // 0
	{ 0, 0, 0xff7f0000, -1, 1, 1}, // 4
	{ 1, 0, 0xff7f0000, 1, 1, 1}, // 5

	{ 0, 1, 0xff7f0000, -1, -1, 1}, // 0
	{ 1, 0, 0xff7f0000, 1, 1, 1}, // 5
	{ 1, 1, 0xff7f0000, 1, -1, 1}, // 1

	{ 1, 0, 0xff7f0000, 1, 1, -1}, // 6
	{ 0, 0, 0xff7f0000, -1, 1, -1}, // 2
	{ 0, 1, 0xff7f0000, -1, -1, -1}, // 3

	{ 1, 1, 0xff7f0000, 1, -1, -1}, // 7
	{ 1, 0, 0xff7f0000, 1, 1, -1}, // 6
	{ 0, 1, 0xff7f0000, -1, -1, -1}, // 3

	{ 0, 1, 0xff007f00, 1, -1, -1}, // 0
	{ 1, 1, 0xff007f00, 1, -1, 1}, // 3
	{ 1, 0, 0xff007f00, 1, 1, 1}, // 7

	{ 0, 1, 0xff007f00, 1, -1, -1}, // 0
	{ 1, 0, 0xff007f00, 1, 1, 1}, // 7
	{ 0, 0, 0xff007f00, 1, 1, -1}, // 4

	{ 0, 1, 0xff007f00, -1, -1, -1}, // 0
	{ 1, 1, 0xff007f00, -1, 1, -1}, // 3
	{ 1, 0, 0xff007f00, -1, 1, 1}, // 7

	{ 0, 1, 0xff007f00, -1, -1, -1}, // 0
	{ 1, 0, 0xff007f00, -1, 1, 1}, // 7
	{ 0, 0, 0xff007f00, -1, -1, 1}, // 4

	{ 0, 1, 0xff00007f, -1, 1, -1}, // 0
	{ 1, 1, 0xff00007f, 1, 1, -1}, // 1
	{ 1, 0, 0xff00007f, 1, 1, 1}, // 2

	{ 0, 1, 0xff00007f, -1, 1, -1}, // 0
	{ 1, 0, 0xff00007f, 1, 1, 1}, // 2
	{ 0, 0, 0xff00007f, -1, 1, 1}, // 3

	{ 0, 1, 0xff00007f, -1, -1, -1}, // 4
	{ 1, 1, 0xff00007f, -1, -1, 1}, // 7
	{ 1, 0, 0xff00007f, 1, -1, 1}, // 6

	{ 0, 1, 0xff00007f, -1, -1, -1}, // 4
	{ 1, 0, 0xff00007f, 1, -1, 1}, // 6
	{ 0, 0, 0xff00007f, 1, -1, -1}, // 5
};

// the vertices for all the tiles
struct vertex __attribute__((aligned(16))) tileVerticesBlack[4*3];
struct vertex __attribute__((aligned(16))) tileVerticesLetterS[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesLetterC[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesLetterO[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesLetterR[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesLetterE[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesLetterL[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesLetterV[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesLetterI[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesLetterN[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesNumber0[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesNumber1[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesNumber2[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesNumber3[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesNumber4[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesNumber5[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesNumber6[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesNumber7[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesNumber8[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesNumber9[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesBlock1[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesBlock2[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesBlock3[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesBlock4[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesBlock5[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesBlock6[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesLetterH[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesLetterP[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesLetterU[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesLetterT[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesLetterA[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesMissile[12*3];
struct vertex __attribute__((aligned(16))) tileVerticesBlackBorder[12*3];

// the tile mesh table
struct vertex *tileVertices[] = {
	NULL,
	tileVerticesWall,
	tileVerticesBlack,
	tileVerticesLetterS,
	tileVerticesLetterC,
	tileVerticesLetterO,
	tileVerticesLetterR,
	tileVerticesLetterE,
	tileVerticesLetterL,
	tileVerticesLetterV,
	tileVerticesLetterI,
	tileVerticesLetterN,
	tileVerticesNumber0,
	tileVerticesNumber1,
	tileVerticesNumber2,
	tileVerticesNumber3,
	tileVerticesNumber4,
	tileVerticesNumber5,
	tileVerticesNumber6,
	tileVerticesNumber7,
	tileVerticesNumber8,
	tileVerticesNumber9,
	tileVerticesBlock1,
	tileVerticesBlock2,
	tileVerticesBlock3,
	tileVerticesBlock4,
	tileVerticesBlock5,
	tileVerticesBlock6,
	tileVerticesLetterH,
	tileVerticesLetterP,
	tileVerticesLetterU,
	tileVerticesLetterT,
	tileVerticesLetterA,
	tileVerticesMissile,
	tileVerticesBlackBorder
};

// the number of triangles one tile has
unsigned int tileTriangles[] = {
	0,  // NONE
	12, // WALL
	4,  // BLACK
	12, // LETTER S
	12, // LETTER C
	12, // LETTER O
	12, // LETTER R
	12, // LETTER E
	12, // LETTER L
	12, // LETTER V
	12, // LETTER I
	12, // LETTER N
	12, // NUMBER 0
	12, // NUMBER 1
	12, // NUMBER 2
	12, // NUMBER 3
	12, // NUMBER 4
	12, // NUMBER 5
	12, // NUMBER 6
	12, // NUMBER 7
	12, // NUMBER 8
	12, // NUMBER 9
	12, // BLOCK 1
	12, // BLOCK 2
	12, // BLOCK 3
	12, // BLOCK 4
	12, // BLOCK 5
	12, // BLOCK 6
	12, // LETTER H
	12, // LETTER P
	12, // LETTER U
	12, // LETTER T
	12, // LETTER A
	2,  // MISSILE
	12, // BLACK BORDER
};

// the main graphics timer
float gfxTimer = 0.0f;

// the pause string
char stringPause[] = {
	TILE_TYPE_LETTER_P,
	TILE_TYPE_LETTER_A,
	TILE_TYPE_LETTER_U,
	TILE_TYPE_LETTER_S,
	TILE_TYPE_LETTER_E
};

// the press start string
char stringPressStart[] = {
	TILE_TYPE_LETTER_P,
	TILE_TYPE_LETTER_R,
	TILE_TYPE_LETTER_E,
	TILE_TYPE_LETTER_S,
	TILE_TYPE_LETTER_S,
	-1,
	TILE_TYPE_LETTER_S,
	TILE_TYPE_LETTER_T,
	TILE_TYPE_LETTER_A,
	TILE_TYPE_LETTER_R,
	TILE_TYPE_LETTER_T
};

// tmp vectors for gfxGameBlocksDraw()
volatile ScePspFVector3 posA = { 0.0f, 0.0f,  0.0f };
volatile ScePspFVector3 posP = { 0.0f, 0.0f, -1.0f };
volatile ScePspFVector3 posB = { 0.0f, 0.0f, -1.0f };
volatile ScePspFVector3 scaA = { 1.0f, 1.0f,  1.0f };
volatile ScePspFVector3 rotA = { 0.0f, 0.0f,  0.0f };
volatile ScePspFVector3 rotB = { 0.0f, 0.0f,  0.0f };
volatile ScePspFVector3 rotM = { 0.0f, 0.0f,  0.0f };

// the preview block scale
float previewBlockScale = 2.0f;

// ---------------------------------------------------------------------------
// recreate the texture coordinates
// ---------------------------------------------------------------------------

void _txtCoordinatesFix(struct vertex *vertices, int n, float sX, float sY, float size) {

	int i;

	size *= TILE_DX;

	// pixels -> uvs
	sX = (1.0f/64.0f) * sX;
	sY = (1.0f/64.0f) * sY;

	for (i = 0; i < n; i++) {
		if (vertices[i].u == 0)
			vertices[i].u = sX + 0.01f;
		else
			vertices[i].u = sX + size;
		if (vertices[i].v == 0)
			vertices[i].v = sY + 0.01f;
		else
			vertices[i].v = sY + size;

		// also reset the color
		vertices[i].color = 0xffffffff;
	}
}

// ---------------------------------------------------------------------------
// copy vertices (used to clone tile meshes)
// ---------------------------------------------------------------------------

void _verticesCopy(struct vertex *template, struct vertex *out, int n) {

	int i = 0;

	while (i < n) {
		out[i].u = template[i].u;
		out[i].v = template[i].v;
		out[i].color = template[i].color;
		out[i].x = template[i].x;
		out[i].y = template[i].y;
		out[i].z = template[i].z;
		i++;
	}
}

// ---------------------------------------------------------------------------
// init the graphics subsystem
// ---------------------------------------------------------------------------

void gfxInit(void) {

	// overclock!
	//scePowerSetClockFrequency(333, 333, 166);

	// inflate the texture
	inflate(texture_start, textureUnpacked);

	// create the tile meshes
	_verticesCopy(tileVerticesWall, tileVerticesBlack, 4*3);
	_verticesCopy(tileVerticesWall, tileVerticesLetterS, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesLetterC, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesLetterO, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesLetterR, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesLetterE, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesLetterL, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesLetterV, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesLetterI, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesLetterN, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesNumber0, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesNumber1, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesNumber2, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesNumber3, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesNumber4, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesNumber5, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesNumber6, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesNumber7, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesNumber8, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesNumber9, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesBlock1, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesBlock2, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesBlock3, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesBlock4, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesBlock5, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesBlock6, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesLetterH, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesLetterP, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesLetterU, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesLetterT, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesLetterA, 12*3);
	_verticesCopy(tileVerticesWall, tileVerticesMissile, 2*3);
	_verticesCopy(tileVerticesWall, tileVerticesBlackBorder, 12*3);

	// reset the texture coordinates
	_txtCoordinatesFix(tileVerticesWall,    12*3, 8*1, 8*0, 1.0f);
	_txtCoordinatesFix(tileVerticesBlack,    4*3, 8*0, 8*0, 1.0f);
	_txtCoordinatesFix(tileVerticesLetterS, 12*3, 8*0, 8*1, 1.0f);
	_txtCoordinatesFix(tileVerticesLetterC, 12*3, 8*1, 8*1, 1.0f);
	_txtCoordinatesFix(tileVerticesLetterO, 12*3, 8*2, 8*1, 1.0f);
	_txtCoordinatesFix(tileVerticesLetterR, 12*3, 8*3, 8*1, 1.0f);
	_txtCoordinatesFix(tileVerticesLetterE, 12*3, 8*4, 8*1, 1.0f);
	_txtCoordinatesFix(tileVerticesLetterL, 12*3, 8*5, 8*1, 1.0f);
	_txtCoordinatesFix(tileVerticesLetterV, 12*3, 8*6, 8*1, 1.0f);
	_txtCoordinatesFix(tileVerticesLetterI, 12*3, 8*7, 8*1, 1.0f);
	_txtCoordinatesFix(tileVerticesLetterN, 12*3, 8*0, 8*2, 1.0f);
	_txtCoordinatesFix(tileVerticesNumber0, 12*3, 8*1, 8*2, 1.0f);
	_txtCoordinatesFix(tileVerticesNumber1, 12*3, 8*2, 8*2, 1.0f);
	_txtCoordinatesFix(tileVerticesNumber2, 12*3, 8*3, 8*2, 1.0f);
	_txtCoordinatesFix(tileVerticesNumber3, 12*3, 8*4, 8*2, 1.0f);
	_txtCoordinatesFix(tileVerticesNumber4, 12*3, 8*5, 8*2, 1.0f);
	_txtCoordinatesFix(tileVerticesNumber5, 12*3, 8*6, 8*2, 1.0f);
	_txtCoordinatesFix(tileVerticesNumber6, 12*3, 8*7, 8*2, 1.0f);
	_txtCoordinatesFix(tileVerticesNumber7, 12*3, 8*0, 8*3, 1.0f);
	_txtCoordinatesFix(tileVerticesNumber8, 12*3, 8*1, 8*3, 1.0f);
	_txtCoordinatesFix(tileVerticesNumber9, 12*3, 8*2, 8*3, 1.0f);
	_txtCoordinatesFix(tileVerticesBlock1,  12*3, 8*2, 8*0, 1.0f);
	_txtCoordinatesFix(tileVerticesBlock2,  12*3, 8*3, 8*0, 1.0f);
	_txtCoordinatesFix(tileVerticesBlock3,  12*3, 8*4, 8*0, 1.0f);
	_txtCoordinatesFix(tileVerticesBlock4,  12*3, 8*5, 8*0, 1.0f);
	_txtCoordinatesFix(tileVerticesBlock5,  12*3, 8*6, 8*0, 1.0f);
	_txtCoordinatesFix(tileVerticesBlock6,  12*3, 8*7, 8*0, 1.0f);
	_txtCoordinatesFix(tileVerticesLetterH, 12*3, 8*3, 8*3, 1.0f);
	_txtCoordinatesFix(tileVerticesLetterP, 12*3, 8*4, 8*3, 1.0f);
	_txtCoordinatesFix(tileVerticesLetterU, 12*3, 8*5, 8*3, 1.0f);
	_txtCoordinatesFix(tileVerticesLetterT, 12*3, 8*6, 8*3, 1.0f);
	_txtCoordinatesFix(tileVerticesLetterA, 12*3, 8*7, 8*3, 1.0f);
	_txtCoordinatesFix(tileVerticesMissile,  2*3, 8*0, 8*4, 2.0f);
	_txtCoordinatesFix(tileVerticesBlackBorder, 12*3, 8*0, 8*0, 1.0f);

	// flush the data cache
	sceKernelDcacheWritebackAll();

	// init the hardware
	void *fbp0 = getStaticVramBuffer(BUFFER_DX, SCREEN_DY, GU_PSM_8888);
	void *fbp1 = getStaticVramBuffer(BUFFER_DX, SCREEN_DY, GU_PSM_8888);
	void *zbp = getStaticVramBuffer(BUFFER_DX, SCREEN_DY, GU_PSM_4444);

	sceGuInit();

	sceGuStart(GU_DIRECT, list);
	sceGuDrawBuffer(GU_PSM_8888, fbp0, BUFFER_DX);
	sceGuDispBuffer(SCREEN_DX, SCREEN_DY, fbp1, BUFFER_DX);
	sceGuDepthBuffer(zbp, BUFFER_DX);
	sceGuOffset(2048 - (SCREEN_DX/2), 2048 - (SCREEN_DY/2));
	sceGuViewport(2048, 2048, SCREEN_DX, SCREEN_DY);
	sceGuDepthRange(65535, 0);
	sceGuScissor(0, 0, SCREEN_DX, SCREEN_DY);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);

	// setup texture
	sceGuTexMode(GU_PSM_8888, 0, 0, 0);
	sceGuTexImage(0, 64, 64, 64, textureUnpacked);
	sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
	sceGuTexEnvColor(0xffff00);
	sceGuTexFilter(GU_LINEAR, GU_LINEAR);
	sceGuTexScale(1.0f, 1.0f);
	sceGuTexOffset(0.0f, 0.0f);
	sceGuAmbientColor(0xffffffff);

	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);
}

// ---------------------------------------------------------------------------
// draw the background graphics
// ---------------------------------------------------------------------------

void gfxGameBackgroundDraw(void) {

	sceGuStart(GU_DIRECT, list);

	// clear screen (ABGR)
	if (gameScore < gameHiScore || gameOver == YES)
		sceGuClearColor(0xffA9652E);
	else
		sceGuClearColor(0xffCBA07E);

	sceGuClearDepth(0);
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
}

// ---------------------------------------------------------------------------
// draw a string (of tiles)
// ---------------------------------------------------------------------------

void gfxDrawString(char *string, int length) {

	float pX = -(length * 2.0f) / 2.0f;
	int i, tile;

	for (i = 0; i < length; i++) {
		tile = string[i];
		if (tile < 0)
			continue;

		// transform
		posA.x = pX + (i * 2.0f);
		posA.y = -(19 << 1) + TILE_MAP_DY;

		sceGumPushMatrix();
		sceGumTranslate((ScePspFVector3 *)&posA);

		// draw a tile
		sceGumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, tileTriangles[tile]*3, 0, tileVertices[tile]);
		sceGumPopMatrix();
	}
}

// ---------------------------------------------------------------------------
// draw the playfield
// ---------------------------------------------------------------------------

void gfxGameBlocksDraw(void) {

	unsigned char *mask;
	int tX, tY, tile;

	// setup matrices
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumPerspective(75.0f, 16.0f/9.0f, 0.5f, 100.0f);

	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();

	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();

	// draw the background particles
	particlesDraw();

	// move the playfield
	sceGumTranslate((ScePspFVector3 *)&posP);

	// rotate the view
	sceGumRotateXYZ((ScePspFVector3 *)&rotB);

	// draw the tiles
	for (tY = 0; tY < TILE_MAP_DY; tY++) {
		for (tX = 0; tX < TILE_MAP_DX; tX++) {
			tile = tileMap[tY*TILE_MAP_DX + tX];

			// skip some tiles
			if (tile == TILE_TYPE_NONE)
				continue;

			// transform
			posA.x = (tX << 1) - TILE_MAP_DX;
			posA.y = -(tY << 1) + TILE_MAP_DY;

			sceGumPushMatrix();
			sceGumTranslate((ScePspFVector3 *)&posA);

			// draw a tile
			sceGumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, tileTriangles[tile]*3, 0, tileVertices[tile]);
			sceGumPopMatrix();
		}
	}

	if (gamePaused == YES) {
		// draw PAUSE
		gfxDrawString(stringPause, 5);
	}
	else if (gameOver == YES) {
		// draw PRESS START
		gfxDrawString(stringPressStart, 11);
	}

	// draw the next block
	tile = gameBlockNext;
	mask = gameBlockGetMask(tile, 0);
	tile = blockTiles[tile];

	for (tY = 0; tY < 4; tY++) {
		for (tX = 0; tX < 4; tX++) {
			if (mask[tY*4 + tX] == 0)
				continue;

			sceGumPushMatrix();

			posB.x = (14 << 1) - TILE_MAP_DX + 1;
			posB.y = -(13 << 1) + TILE_MAP_DY;
			posB.z = posA.z + 5.0f;
			sceGumTranslate((ScePspFVector3 *)&posB);

			sceGumRotateXYZ((ScePspFVector3 *)&rotA);

			scaA.x = previewBlockScale;
			scaA.y = previewBlockScale;
			scaA.z = previewBlockScale;
			sceGumScale((ScePspFVector3 *)&scaA);

			posB.x = (tX << 1) - 3.0f;
			posB.y = -(tY << 1) + 3.0f;
			posB.z = 0.0f;
			sceGumTranslate((ScePspFVector3 *)&posB);
			posB.z = posA.z + 6.0f;

			sceGumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, tileTriangles[tile]*3, 0, tileVertices[tile]);
			sceGumPopMatrix();
		}
	}
}

// ---------------------------------------------------------------------------
// handle background rotations and zooming
// ---------------------------------------------------------------------------

void gfxGameBackgroundLive(float deltaTime) {

	unsigned int keys = ioKeyBits;

	gfxTimer += deltaTime;

	if (posP.z > -30.0f) {
		posP.z -= deltaTime / 120.0f;
		if (posP.z < -30.0f)
			posP.z = -30.0f;
	}

	// rotate the preview block
	float r = gfxTimer / 20.0f;

	rotA.x = r * 0.3f * (GU_PI/180.0f);
	rotA.y = r * 0.7f * (GU_PI/180.0f);
	rotA.z = r * 1.3f * (GU_PI/180.0f);

	// zoom the preview block
	if (previewBlockScale > 1.0f) {
		previewBlockScale -= deltaTime / 60.0f;
		if (previewBlockScale < 1.0f)
			previewBlockScale = 1.0f;
	}

	// rotate the view
	if ((keys & IO_KEY_BIT_R) != 0)
		rotB.z -= deltaTime / 1000.0f;
	if ((keys & IO_KEY_BIT_L) != 0)
		rotB.z += deltaTime / 1000.0f;

	if ((keys & IO_KEY_BIT_X) != 0)
		rotB.x -= deltaTime / 1000.0f;
	if ((keys & IO_KEY_BIT_B) != 0)
		rotB.x += deltaTime / 1000.0f;

	if ((keys & IO_KEY_BIT_Y) != 0) {
		// reset the view rotation
		rotB.z = 0.0f;
		rotB.x = 0.0f;
	}
}

// ---------------------------------------------------------------------------
// swap the draw buffers
// ---------------------------------------------------------------------------

void gfxSwapBuffers(void) {

	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
}

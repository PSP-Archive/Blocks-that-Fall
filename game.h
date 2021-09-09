
#ifndef GAME_H
#define GAME_H

// tile types
#define TILE_TYPE_NONE          0
#define TILE_TYPE_WALL          1
#define TILE_TYPE_BLACK         2
#define TILE_TYPE_LETTER_S      3
#define TILE_TYPE_LETTER_C      4
#define TILE_TYPE_LETTER_O      5
#define TILE_TYPE_LETTER_R      6
#define TILE_TYPE_LETTER_E      7
#define TILE_TYPE_LETTER_L      8
#define TILE_TYPE_LETTER_V      9
#define TILE_TYPE_LETTER_I     10
#define TILE_TYPE_LETTER_N     11
#define TILE_TYPE_NUMBER_0     12
#define TILE_TYPE_NUMBER_1     13
#define TILE_TYPE_NUMBER_2     14
#define TILE_TYPE_NUMBER_3     15
#define TILE_TYPE_NUMBER_4     16
#define TILE_TYPE_NUMBER_5     17
#define TILE_TYPE_NUMBER_6     18
#define TILE_TYPE_NUMBER_7     19
#define TILE_TYPE_NUMBER_8     20
#define TILE_TYPE_NUMBER_9     21
#define TILE_TYPE_BLOCK_1      22
#define TILE_TYPE_BLOCK_2      23
#define TILE_TYPE_BLOCK_3      24
#define TILE_TYPE_BLOCK_4      25
#define TILE_TYPE_BLOCK_5      26
#define TILE_TYPE_BLOCK_6      27
#define TILE_TYPE_LETTER_H     28
#define TILE_TYPE_LETTER_P     29
#define TILE_TYPE_LETTER_U     30
#define TILE_TYPE_LETTER_T     31
#define TILE_TYPE_LETTER_A     32
#define TILE_TYPE_MISSILE      33
#define TILE_TYPE_BLACK_BORDER 34

// tile map
#define TILE_MAP_DX (160/8)
#define TILE_MAP_DY (144/8)

// the tile map
extern unsigned char tileMap[TILE_MAP_DY * TILE_MAP_DX];

// block types
#define GAME_BLOCK_TYPE_I 0
#define GAME_BLOCK_TYPE_J 1
#define GAME_BLOCK_TYPE_L 2
#define GAME_BLOCK_TYPE_O 3
#define GAME_BLOCK_TYPE_S 4
#define GAME_BLOCK_TYPE_T 5
#define GAME_BLOCK_TYPE_Z 6

// the next block
extern int gameBlockNext;

// score
extern unsigned int gameScore;

// hi score
extern unsigned int gameHiScore;

// has the game ended?
extern int gameOver;

// is the game paused?
extern int gamePaused;

// each block is drawn using one tile
extern unsigned char blockTiles[];

void gameInit(void);
void gameLive(float deltaTime);
void gameBlockGiveNext(void);
void gameBlockDraw(int bX, int bY, int type, int rotation);
void gameBlockRemove(int bX, int bY, int type, int rotation);
int  gameBlockTest(int bX, int bY, int type, int rotation);
void gameValueDraw(int value, int vX, int vY, int divider);

unsigned char *gameBlockGetMask(int type, int rotation);

#endif

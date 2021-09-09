
#ifndef GFX_H
#define GFX_H

// display buffer, and screen size
#define BUFFER_DX 512
#define SCREEN_DX 480
#define SCREEN_DY 272

// tile size in the 64x64 tile texture
#define TILE_DX ((1.0f/64.0f)*7.5f)

// 3D vertex
struct vertex {
	float u, v;
	unsigned int color;
	float x, y, z;
};

// the preview block scaling
extern float previewBlockScale;

void gfxInit(void);
void gfxGameBackgroundLive(float deltaTime);
void gfxGameBackgroundDraw(void);
void gfxGameBlocksDraw(void);
void gfxSwapBuffers(void);

#endif


/*
 *
 * particle.c - particles
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

#include "main.h"
#include "gfx.h"
#include "particle.h"
#include "random.h"

// ---------------------------------------------------------------------------
// the variables
// ---------------------------------------------------------------------------

// particles
struct particle particles[PARTICLES_MAX];

// index of the first free particle
int particleFree;

// when the life time reaches this value (or is less), the particles are shown.
// give bigger life times to particlesAdd() to get delayed particle births...
const int particleLifeTimes[] = {
   0, // DEAD
  -1, // MISSILE
};

// the tmp vectors used by particlesDraw()
volatile static ScePspFVector3 pos = { 0.0f, 0.0f, 0.0f };
volatile static ScePspFVector3 rot = { 0.0f, 0.0f, 0.0f };

// the missile mesh
extern struct vertex tileVerticesMissile[12*3];

// ---------------------------------------------------------------------------
// init the particles
// ---------------------------------------------------------------------------

void particlesInit(void) {

  int p;

  for (p = 0; p < PARTICLES_MAX; p++) {
		particles[p].type = PARTICLE_TYPE_DEAD;
		particles[p].nextFree = p + 1;
	}

	// after the last one we don't have anything free
	particles[PARTICLES_MAX - 1].nextFree = -1;

	// pointer to the first one
	particleFree = 0;
}

// ---------------------------------------------------------------------------
// advance the particles
// ---------------------------------------------------------------------------

void particlesLive(float deltaTime) {

	struct particle *particle;
	float pX, pY;
  int p;

  for (p = 0; p < PARTICLES_MAX; p++) {
    // skip dead particles
		if (particles[p].type == PARTICLE_TYPE_DEAD)
			continue;

		particle = &particles[p];

		// live
		if (particle->life > 0 && particleLifeTimes[particle->type] > 0)
			particle->life -= deltaTime;

		// skip delayed particles
		if (particleLifeTimes[particle->type] < particle->life)
			continue;

		// did the particle die?
		if ((particle->life <= 0 && particleLifeTimes[particle->type] > 0) || particle->pX > 16*3) {
			// yes -> kill the particle

			// return it to the pool
			particle->type = PARTICLE_TYPE_DEAD;
			particle->nextFree = particleFree;
			particleFree = p;
		}
		else {
			// no, move
			pX = particle->pX + deltaTime*particle->speed/180.0f;
			pY = particle->pY + deltaTime*particle->speed/180.0f;

			// store the new position
			particle->pX = pX;
			particle->pY = pY;
		}
	}
}

// ---------------------------------------------------------------------------
// draw the particles
// ---------------------------------------------------------------------------

void particlesDraw(void) {

	int p;

	sceGuEnable(GU_BLEND);

  for (p = 0; p < PARTICLES_MAX; p++) {
    // skip dead particles
		if (particles[p].type == PARTICLE_TYPE_DEAD)
			continue;

		sceGumPushMatrix();

		pos.x = particles[p].pX;
		pos.y = particles[p].pY;
		pos.z = -35;
		sceGumTranslate((ScePspFVector3 *)&pos);

		rot.z = -M_PI * 0.25f;
		sceGumRotateXYZ((ScePspFVector3 *)&rot);

		// draw a tile
		sceGumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 2*3, 0, tileVerticesMissile);
		sceGumPopMatrix();
	}

	sceGuDisable(GU_BLEND);
}

// ---------------------------------------------------------------------------
// add a particle to the particle pool
// ---------------------------------------------------------------------------

void particleAdd(float pX, float pY, int type) {

	struct particle *particle;

	// out of free particles?
	if (particleFree < 0)
		return;

	particle = &particles[particleFree];
	particleFree = particle->nextFree;

	particle->pX = pX;
	particle->pY = pY;
	particle->life = particleLifeTimes[type];
	particle->type = type;
	particle->speed = 1.0f + ((randomNextInt() & 7) / 21.0f);
}

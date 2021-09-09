
#ifndef PARTICLE_H
#define PARTICLE_H

// maximum number of particles alive
#define PARTICLES_MAX 256

// particle types
#define PARTICLE_TYPE_DEAD    0
#define PARTICLE_TYPE_MISSILE 1

struct particle {
	float pX;
	float pY;
	float speed;
	float life;
	int   type;
	int   nextFree;
};

void particlesInit(void);
void particlesLive(float deltaTime);
void particlesDraw(void);
void particleAdd(float pX, float pY, int type);

#endif

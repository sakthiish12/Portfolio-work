#ifndef _PARTICLE_POOL_H_
#define _PARTICLE_POOL_H_

#include "../../../Math/Vector/Vector2.h"

struct Emitter; //the include below, might move to cpp, while this declar remain
struct Particle;
struct ParticleGenerationProperties;

//This class will be constructed in each Emitter
class ParticlePool
{
private:
	Emitter* ParentPointer = nullptr;
	//int poolSize = (particleMaxLife + 1) * emissionRate;
	int poolSize = 0;
	Particle* firstAvailable;
	Particle* particleArray = nullptr;

	//Statistic for debugging if needed
	int aliveParticle = 0;
	static int totalAliveParticle;
	static int totalPoolSizeParticle;
public:
	ParticlePool(Emitter* emitter = nullptr);
	~ParticlePool();

	// Generates a new particle each time it's called
	//void Generate(Mathf::vec2 pos, float startSpeed, float endSpeed, float angle, float rotSpeed, float startSize, float endSize, uint life, SDL_Rect textureRect, SDL_Color startColor, SDL_Color endColor, SDL_BlendMode blendMode, bool vortexSensitive);
	Particle* Generate(ParticleGenerationProperties& newParticleProperties);

	// Update (move and draw) particles in the pool.
	// IMPORTANT NOTE: This method returns:
	//		- PARTICLE_ALIVE_DRAWN: if the particle is still alive and has been drawn succesfully
	//      - PARTICLE_ALIVE_NOT_DRAWN:  if the particle is alive and has NOT been drawn succesfully
	//		- PARTICLE_DEAD: if is particle is no longer alive

	bool Update(float dt);
	unsigned int GetAliveParticle();
};

#endif

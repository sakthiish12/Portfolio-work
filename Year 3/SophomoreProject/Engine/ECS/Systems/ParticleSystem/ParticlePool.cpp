#include "ParticlePool.h"
#include "Emitter.h"
#include "Particle.h"
#include <iostream>
ParticlePool::ParticlePool(Emitter* emitter)
{
	if (emitter != nullptr)
	{
		//saving pointer to emitter (for future access of other data)
		ParentPointer = emitter;

		// Fill the pool according to poolSize needed for the emitter
		poolSize = emitter->GetPoolSize();
		particleArray = new Particle[poolSize];

		// The first particle is available
		firstAvailable = &particleArray[0];

		// Each particle points to the next one
		for (int i = 0; i < poolSize - 1; i++)
			particleArray[i].SetNext(&particleArray[i + 1]);

		// The last particle points to nullptr indicating the end of the vector
		particleArray[poolSize - 1].SetNext(nullptr);
	}
}


// Generates a new particle each time it's called
//void Generate(Mathf::vec2 pos, float startSpeed, float endSpeed, float angle, float rotSpeed, float startSize, float endSize, uint life, SDL_Rect textureRect, SDL_Color startColor, SDL_Color endColor, SDL_BlendMode blendMode, bool vortexSensitive);
Particle* ParticlePool::Generate(ParticleGenerationProperties& newParticleProperties)
{
	
	//std::cout << __func__ << " is called... with " << aliveParticle + 1  << " aliveParticle" << std::endl;
	//std::cout << "firstAvailable is " << static_cast<void*>(firstAvailable) << std::endl;

	static_cast<void>(newParticleProperties);//unref

	// Check if the pool is not full
	assert(firstAvailable != nullptr);

	// Remove it from the available list
	Particle* newParticle = firstAvailable;
	firstAvailable = newParticle->GetNext();
	//newParticle->SetNext(nullptr); // precautious, so particlePool can detect.
	//newParticle->lifespan = 0.1f;  // precautious, so can set to dead if lifespan is not set by emitter
	++aliveParticle;
	// Initialize new alive particle=======================================================================
	//newParticle->Init(startPos, speed, pRect);
	//newParticle->Init(_newParticleProperties.spawnPos, _newParticleProperties.speed);
	//newParticle->_state.aliveData.pos = { _newParticleProperties.spawnPos.x, _newParticleProperties.spawnPos.y, _newParticleProperties.spawnPos.z };
	//newParticle->_state.aliveData.color = { _newParticleProperties.startColor.r, _newParticleProperties.startColor.g, _newParticleProperties.startColor.b, _newParticleProperties.startColor.a };
	//newParticle->_state.aliveData.scale = { _newParticleProperties.scale.x, _newParticleProperties.scale.y };
	//newParticle->_state.aliveData.vel = { _newParticleProperties.vel.x, _newParticleProperties.vel.y };


	return newParticle;
}

// Update (move and draw) particles in the pool.
// IMPORTANT NOTE: This method returns:
//		- PARTICLE_ALIVE_DRAWN: if the particle is still alive and has been drawn succesfully
//      - PARTICLE_ALIVE_NOT_DRAWN:  if the particle is alive and has NOT been drawn succesfully
//		- PARTICLE_DEAD: if is particle is no longer alive

bool ParticlePool::Update(float dt)
{
	bool ret = false;

	for (int i = 0; i < poolSize; i++)
	{
		//std::cout << "Loop : " << i << std::endl;
		if (particleArray[i].IsAlive() && particleArray[i]._isAlive)
		{
			//updatelife
			particleArray[i].lifespan -= dt;

			particleArray[i].Update(dt);
			particleArray[i].Draw();
			ret = true; // might be useless??

			//if (particleArray[i].lifespan < 0)
			if (!particleArray[i].IsAlive())
			{
				particleArray[i]._isAlive = false;
				--aliveParticle;
				particleArray[i].SetNext(firstAvailable);
				firstAvailable = &(particleArray[i]);
				
				//std::cout << "aliveParticle reduced to " << aliveParticle << std::endl;
				//std::cout << "firstAvailable is " << static_cast<void*>(firstAvailable) << std::endl;
				
			}
		}
		else // if a particle dies it becomes the first available in the pool
		{
			// Add this particle to the front of the vector
			if (particleArray[i].GetNext() == nullptr)
			{
				particleArray[i].SetNext(firstAvailable);
				firstAvailable = &particleArray[i];

				particleArray[i]._isAlive = false;
				//std::cout << "It entered here? How?? " << std::endl; // none entered here
			}
		}
	}
	return ret;
}

ParticlePool::~ParticlePool()
{
	if (particleArray != nullptr)
		delete[] particleArray;
}

unsigned int ParticlePool::GetAliveParticle()
{
	return aliveParticle;
}

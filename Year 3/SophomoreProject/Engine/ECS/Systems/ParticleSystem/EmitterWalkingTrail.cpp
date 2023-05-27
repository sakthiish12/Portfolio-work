#include "EmitterWalkingTrail.h"
#include "../../../Dependencies/Random.h"
#include "../../../Math/Matrix/Matrix3.h"
#include <iostream>
#include "ParticlePool.h"

EmitterWalkingTrail::EmitterWalkingTrail() :
    Emitter{ "EmitterWalkingTrail" }
{
	init();
}

void EmitterWalkingTrail::init()
{
	_emitterData.alwaysActive = false;
	_emitterData.spawnTrigger = true;
    _emitterData.spwanUsingEmitterPos = false;
    _emitterData.NumOfSpawnPerTrigger = 1;
    
    _emitterData.maxParticle = 20;

    //_newParticleProperties.scale = { 50.f, 50.f }; //walkingtrail.png
    _newParticleProperties.scale = { 10.f, 10.f }; //walkingtrail.png
    //_newParticleProperties.textureId = "walkingtrail.png";
    _newParticleProperties.startColor = { 1.0f, 1.0f, 1.0f, 0.5f };
    _newParticleProperties.textureId = "walkingtrail.png";
    _newParticleProperties.lifespan = 0.3f;

    //TODO particle pool need to have a resize fn.

    //to set the constant emission
    _emitterData.emitter_Timer = 0.f; // not used here
    _emitterData.emmisionRate = _newParticleProperties.lifespan / _emitterData.maxParticle;
}

//Behavoir:
//Emitter walking trail will spawn particle, that does not move and fade away as the player move away
//TODO!!!
void EmitterWalkingTrail::update(float dt)
{
	Emitter::updateLife(dt);

    //If alive particle is not more than the max particle
    //and is triggered based
    if (PP->GetAliveParticle() < static_cast<unsigned int>(_emitterData.maxParticle) &&
        isTriggered == true)
    {
        //reset trigger
        isTriggered = false; // reset it  because emit as long as the thing click
        
        //offset neg y to simulate starting from the leg
        _newParticleProperties.spawnPos.y -= 20.f;

        //number of spawn allowed per trigger
        for (int i = 0; i < _emitterData.NumOfSpawnPerTrigger; ++i)
        {
            //check not over max particle
            if (PP->GetAliveParticle() < static_cast<unsigned int>(_emitterData.maxParticle))
            {
                Particle* pPtr = PP->Generate(_newParticleProperties);
                if (pPtr)
                {
                    pPtr->Init(_newParticleProperties.spawnPos, 0.f, _newParticleProperties.lifespan, _newParticleProperties.scale);

                    pPtr->_state.aliveData.vel = { -_movementVel.x * 0.1f * (0.5f + (float)std::rand() / (float)RAND_MAX), -_movementVel.y * 0.1f + 10.0f * ((float)std::rand() / (float)RAND_MAX)}; //vel should be opposite of player waking dir

                    pPtr->_state.aliveData.color = { _newParticleProperties.startColor.r, _newParticleProperties.startColor.g, _newParticleProperties.startColor.b, _newParticleProperties.startColor.a };

                    pPtr->textureId = _newParticleProperties.textureId;
                }
            }
        }
    }

    //then update life
    PP->Update(dt);
}

void EmitterWalkingTrail::setTriggered(bool newState, vec2& movementVel, vec3& particleSpawnAt)
{
    _newParticleProperties.spawnPos = particleSpawnAt;
    _movementVel = movementVel;
	isTriggered = newState;
}

std::shared_ptr<Emitter> EmitterWalkingTrail::createCopy()
{
    return std::make_shared<EmitterWalkingTrail>();
}


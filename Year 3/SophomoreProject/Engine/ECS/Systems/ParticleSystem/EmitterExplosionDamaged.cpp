#include "EmitterExplosionDamaged.h"
#include "../../../Dependencies/Random.h"
#include "../../../Math/Matrix/Matrix3.h"
#include <iostream>
#include "ParticlePool.h"

EmitterExplosionDamaged::EmitterExplosionDamaged():
    Emitter{ "EmitterExplosionDamaged" }
{
	init();
}

void EmitterExplosionDamaged::init()
{
	_emitterData.alwaysActive = false;
	_emitterData.spawnTrigger = true;
    _emitterData.spwanUsingEmitterPos = false;
    _emitterData.NumOfSpawnPerTrigger = 20;
    
    _emitterData.maxParticle = 20;

    //_newParticleProperties.scale = { 50.f, 50.f }; //walkingtrail.png
    _newParticleProperties.scale = { 10.f, 10.f }; //walkingtrail.png
    //_newParticleProperties.textureId = "walkingtrail.png";
    _newParticleProperties.use_texture = true;
    _newParticleProperties.startColor = { 1.0f, 1.0f, 0.5f, 0.8f };
    _newParticleProperties.textureId = "star.png";
    _newParticleProperties.lifespan = 0.3f;

    //TODO particle pool need to have a resize fn.

    //to set the constant emission
    _emitterData.emitter_Timer = 0.f; // not used here
    _emitterData.emmisionRate = _newParticleProperties.lifespan / _emitterData.maxParticle;
}

void EmitterExplosionDamaged::update(float dt)
{
	Emitter::updateLife(dt);

    //If alive particle is not more than the max particle
    //and is triggered based
    if (PP->GetAliveParticle() < static_cast<unsigned int>(_emitterData.maxParticle) &&
        isTriggered == true)
    {
        //reset trigger
        isTriggered = false; // reset it  because emit as long as the thing click
        
        //number of spawn allowed per trigger
        for (int i = 0; i < _emitterData.NumOfSpawnPerTrigger; ++i)
        {
            if (PP->GetAliveParticle() < static_cast<unsigned int>(_emitterData.maxParticle))
            {
                Particle* pPtr = PP->Generate(_newParticleProperties);
                if (pPtr)
                {
                    pPtr->Init(_newParticleProperties.spawnPos, _newParticleProperties.speed, _newParticleProperties.lifespan, _newParticleProperties.scale);

                    //pick random arc range to start
                    float randomArcRange = Random::Range<float>(0.f, 3.141592654f * 2.f);

                    //find vel required
                    vec3 to_randomArcRange{ 0.0f, suctionLimit, 0.0f };
                    float cosRot = cos(randomArcRange);
                    float sinRot = sin(randomArcRange);
                    Mathf::Matrix3 rotate{
                        cosRot, sinRot, 0.0f,
                        -sinRot, cosRot, 0.0f,
                        0.0f, 0.0f, 1.0f
                    };
                    to_randomArcRange = rotate * to_randomArcRange;
                    pPtr->_state.aliveData.rotation = randomArcRange;
                    pPtr->_state.aliveData.pos = pos;

                    pPtr->_state.aliveData.color = { _newParticleProperties.startColor.r, _newParticleProperties.startColor.g, _newParticleProperties.startColor.b, _newParticleProperties.startColor.a };
                    pPtr->_state.aliveData.scale = { _newParticleProperties.scale.x, _newParticleProperties.scale.y };

                    //from spawned position towards player center
                    vec3 velRequired = to_randomArcRange;
                    //divide by lifespan to die at center
                    velRequired.x /= pPtr->lifespan;
                    velRequired.y /= pPtr->lifespan;

                    pPtr->_state.aliveData.vel = velRequired;
                    //std::cout << "velRequired x: " << velRequired.x << " y: " << velRequired.y << std::endl;

                    pPtr->use_texture = true;
                    pPtr->textureId = _newParticleProperties.textureId;
                }
            }
        }
    }
    //then update
    PP->Update(dt);
}

void EmitterExplosionDamaged::setTriggered(bool newState)
{
	isTriggered = newState;
}

std::shared_ptr<Emitter> EmitterExplosionDamaged::createCopy()
{
    return std::make_shared<EmitterExplosionDamaged>();
}

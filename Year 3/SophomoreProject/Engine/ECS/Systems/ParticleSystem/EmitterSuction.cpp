#include "EmitterSuction.h"
#include "../../../Dependencies/Random.h"
#include "../../../Math/Matrix/Matrix3.h"
#include <iostream>

EmitterSuction::EmitterSuction() :
    Emitter{ "EmitterSuction" }
{
	init();
}

void EmitterSuction::init()
{
	_emitterData.alwaysActive = true;
	_emitterData.spawnTrigger = true;
    _emitterData.spwanUsingEmitterPos = false;
    _emitterData.maxParticle = 20; 

    _newParticleProperties.scale = { 20.f, 20.f };
    _newParticleProperties.textureId = "walkingtrail.png";
    _newParticleProperties.lifespan = 0.3f;

    //TODO particle pool need to have a resize fn.

    //to set the constant emission
    _emitterData.emitter_Timer = 0.f; // not used here
    _emitterData.emmisionRate = _newParticleProperties.lifespan / _emitterData.maxParticle;
}

void EmitterSuction::update(float dt)
{
	Emitter::updateLife(dt);

    //emit or update particle
    //if can emit, emit
    if (PP->GetAliveParticle() < static_cast<unsigned int>(_emitterData.maxParticle) &&
        suctionTriggered == true)
    {
        suctionTriggered = false; // reset it  because emit as long as the thing click

        if (PP->GetAliveParticle() < static_cast<unsigned int>(_emitterData.maxParticle))
        {
            Particle* pPtr = PP->Generate(_newParticleProperties);
            //pPtr->Init(_newParticleProperties.spawnPos, _newParticleProperties.speed, 1.f);
            if (pPtr)
            {
                pPtr->Init(_newParticleProperties.spawnPos, _newParticleProperties.speed, _newParticleProperties.lifespan);

                //pick random arc range to start
                float randomArcRange = Random::Range<float>(targetAngle - angleArc, targetAngle + angleArc);

                //find pos, move by suctionLimit in dir of randomArcRange
                vec3 to_randomArcRange{ 0.0f, suctionLimit, 0.0f };
                float cosRot = cos(randomArcRange);
                float sinRot = sin(randomArcRange);
                Mathf::Matrix3 rotate{
                    cosRot, sinRot, 0.0f,
                    -sinRot, cosRot, 0.0f,
                    0.0f, 0.0f, 1.0f
                };
                to_randomArcRange = rotate * to_randomArcRange;

                pPtr->_state.aliveData.pos = pos + to_randomArcRange;

                pPtr->_state.aliveData.color = { _newParticleProperties.startColor.r, _newParticleProperties.startColor.g, _newParticleProperties.startColor.b, _newParticleProperties.startColor.a };
                pPtr->_state.aliveData.scale = { _newParticleProperties.scale.x, _newParticleProperties.scale.y };

                //from spawned position towards player center
                vec3 velRequired = pos - pPtr->_state.aliveData.pos;
                //divide by lifespan to die at center
                velRequired.x /= pPtr->lifespan;
                velRequired.y /= pPtr->lifespan;

                pPtr->_state.aliveData.vel = velRequired;
                pPtr->textureId = _newParticleProperties.textureId;
                //pPtr->lifespan = _newParticleProperties.lifespan;
            }
        }
    }
    //then update
    PP->Update(dt);
}

void EmitterSuction::setSuctionTriggered(bool newState)
{
	suctionTriggered = newState;
}

void EmitterSuction::setSuctionTriggered(bool newState, float target_angle, float angle_arc, float radiusLen)
{
    suctionTriggered = newState;
    targetAngle = target_angle;
    angleArc = angle_arc;
    suctionLimit = radiusLen;
}

std::shared_ptr<Emitter> EmitterSuction::createCopy()
{
    return std::make_shared<EmitterSuction>();
}

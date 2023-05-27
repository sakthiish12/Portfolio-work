// /******************************************************************************/
// /*!
// \file         .cpp
// \project name Engine
// \author(s)    Sakthiish
// \date         10th Apr, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the pause quit game script 
// \lines        
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "EmitterDig.h"
#include "../../../Dependencies/Random.h"
#include "../../../Math/Matrix/Matrix3.h"
#include <iostream>
#include "ParticlePool.h"

EmitterDig::EmitterDig() :
    Emitter{ "EmitterDig" }
{
    init();
}

void EmitterDig::init()
{
    _emitterData.alwaysActive = false;
    _emitterData.spawnTrigger = true;
    _emitterData.spwanUsingEmitterPos = false;
    _emitterData.NumOfSpawnPerTrigger = 2;

    _emitterData.maxParticle = 20;

    
    _newParticleProperties.scale = { 25.f, 25.f }; //walkingtrail.png
    _newParticleProperties.startColor = { 1.f,1.f,1.f,1.f };
    _newParticleProperties.textureId = "Cave Stone 5.png";
    _newParticleProperties.lifespan = 0.3f;

    //TODO particle pool need to have a resize fn.

    //to set the constant emission
    _emitterData.emitter_Timer = 0.f; // not used here
    _emitterData.emmisionRate = _newParticleProperties.lifespan / _emitterData.maxParticle;
}

//Behavoir:
//Emitter walking trail will spawn particle, that does not move and fade away as the player move away
//TODO!!!
void EmitterDig::update(float dt)
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
        //_newParticleProperties.spawnPos.y -= 5.f;
        _newParticleProperties.spawnPos.z = 1.5f;

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
                    pPtr->_state.aliveData.pos.z = 1.5f;
                    pPtr->_state.aliveData.color = { _newParticleProperties.startColor.x ,_newParticleProperties.startColor.y,_newParticleProperties.startColor.z,_newParticleProperties.startColor.w }; //vel should be opposite of player waking dir

                    pPtr->_state.aliveData.vel = { -_movementVel.x * 0.1f, -_movementVel.y * 0.1f }; //vel should be opposite of player waking dir

                    pPtr->textureId = _newParticleProperties.textureId;
                }
            }
        }
    }
    //then update life
    PP->Update(dt);
}

void EmitterDig::setTriggered(bool newState, vec2& movementVel, vec3& particleSpawnAt)
{
    _newParticleProperties.spawnPos = particleSpawnAt;
    _movementVel = movementVel;
    isTriggered = newState;
}

std::shared_ptr<Emitter> EmitterDig::createCopy()
{
    return std::make_shared<EmitterDig>();
}


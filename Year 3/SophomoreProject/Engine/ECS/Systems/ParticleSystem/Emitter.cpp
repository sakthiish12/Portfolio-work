#include "Emitter.h"
#include "Particle.h"
#include "../../../Dependencies/Random.h"
#include "ParticlePool.h"

std::shared_ptr<Emitter> Emitter::createCopy()
{
    return std::shared_ptr<Emitter>(this);
}

Emitter::Emitter(std::string name_) :
    name{ name_ },
    PP{ std::make_shared<ParticlePool>(this) }
{
}



void Emitter::update(float dt)
{
    //
    if (updateLife(dt) == false)
        return;

    //emit or update particle
    //if can emit, emit
    if (PP->GetAliveParticle() < static_cast<unsigned int>(_emitterData.maxParticle))
    {
        //emit at emission rate
        _emitterData.emitter_Timer += dt;
        if (_emitterData.emitter_Timer > _emitterData.emmisionRate && _emitterData.lifespan > 0.f)
        {
            Particle* pPtr = PP->Generate(_newParticleProperties);
            if (pPtr)
            {
                pPtr->Init(_newParticleProperties.spawnPos, _newParticleProperties.speed);
                pPtr->_state.aliveData.pos = { _newParticleProperties.spawnPos.x, _newParticleProperties.spawnPos.y, _newParticleProperties.spawnPos.z };
                pPtr->_state.aliveData.color = { _newParticleProperties.startColor.r, _newParticleProperties.startColor.g, _newParticleProperties.startColor.b, _newParticleProperties.startColor.a };
                pPtr->_state.aliveData.scale = { _newParticleProperties.scale.x, _newParticleProperties.scale.y };
                pPtr->_state.aliveData.vel = { _newParticleProperties.vel.x, _newParticleProperties.vel.y };
            }
            _emitterData.emitter_Timer -= 1.f / _emitterData.emmisionRate;
        }
    }
    //then update
    PP->Update(dt);
}

bool Emitter::updateLife(float dt)
{
    if (_emitterData.alwaysActive == true)
        return true; //always alive

    _emitterData.lifespan -= dt; //update age

    if (_emitterData.lifespan < 0.f && PP->GetAliveParticle() < 1)
        return false; //dead
    return true; // alive since not dead
}

int Emitter::GetPoolSize()
{
    //Need to do calculation so as to ensure it is at near optimal level
    return _emitterData.maxParticle;
}

inline std::string Emitter::getScriptName()
{
    return name;
}

std::string Emitter::getPath()
{
    return path;
}

void Emitter::changePath(std::string newPath)
{
    path = newPath;
}

std::string Emitter::getIDname()
{
    return IDname;
}

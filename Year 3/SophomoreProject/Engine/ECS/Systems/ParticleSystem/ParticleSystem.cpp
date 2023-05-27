#include "ParticleSystem.h"
#include "../../../Graphics/Source/BatchRenderer2D.h"
#include "../../ECS.h"
#include "../../component.h"
#include "IncludeAllEmitter.h"

std::shared_ptr<ParticleSystem> particleSystemPtr;

ParticleSystem::ParticleSystem(bool requireComponentMatch) :
	systemBase(requireComponentMatch)
{

}

void ParticleSystem::init()
{
	ecs.registerComponentType<EmitterComponent>();
	ecs.setSystemComponentRequirement<ParticleSystem, EmitterComponent>();
	ecs.setSystemComponentRequirement<ParticleSystem, transform2D>();

	IncludeAllEmitter();
	//ParticleTesting(); //To test when starting straight
}

void ParticleSystem::update(float dt)
{
	for (auto const& e : getEntitiesRegistered())
	{
		auto& pec = ecs.getComponent<EmitterComponent>(e);
		auto& trans = ecs.getComponent<transform2D>(e);

		for (auto& emi : pec.emitterContainer)
		{
			//update emitter, which will also emit/update particle
			if (emi.second != nullptr)
			{
				emi.second->id = e;
				emi.second->pos.z = 1.5;
				emi.second->pos = trans.pos;

				emi.second->update(dt);
			}
		}
		
	}
}

void ParticleSystem::destoryEmitter()
{
}

void ParticleSystem::destoryAllEmitter()
{
}

void ParticleSystem::ParticleTesting()
{
	ecs.addComponent<transform2D>(2, transform2D{});
	ecs.addComponent<EmitterComponent>(2, EmitterComponent{}, "EmitterTestingEntity");
}

void emitterManager::addEmitter(std::shared_ptr<Emitter> emi)
{
	allEmitterContainer[emi->name] = emi;
}

void emitterManager::deleteEmitter(std::shared_ptr<Emitter> emi)
{
	deleteEmitter(emi->name);
}

void emitterManager::deleteEmitter(std::string emi)
{
	allEmitterContainer.erase(emi);
}

emitterManager& emitterManager::getInstance()
{
	static emitterManager inst;
	return inst;
}

std::shared_ptr<Emitter> emitterManager::createFromEmitterDatabase(std::string name)
{
	if (allEmitterContainer[name] == nullptr)
	{
		//should i even do anything here if based on single responsibility principle?

		//attempt serialise and add to database

		//else return nullptr
		return nullptr;
	}
	return allEmitterContainer[name]->createCopy();
}

std::shared_ptr<Emitter> emitterManager::getEmitter(std::string name)
{
	return allEmitterContainer[name];
}

std::map<std::string, std::shared_ptr<Emitter>>& emitterManager::getEmitterDatabase()
{
	return allEmitterContainer;
}

#include "../../../Engine/ECS/component.h"
#include "../../../Engine/ECS/ECS.h"
#include "../../../Engine/ECS/Systems/Logic.h"
#include "ThroneRoomTeleporterScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/ECS/Systems/ParticleSystem/EmitterTeleporter.h"
#include <iostream>
#include <random>
#include <Sound/SoundManager.h>
#include <Sound/SoundManagerV2.h>


using std::cout;
using std::endl;

/**
\brief - random number generator for integers
\param start - the starting integer range
\param end - the ending integer range
\returns - the random number within the range
**/
template<typename T>
T randomnumgen(T start, T end) {
	std::random_device rd;
	std::mt19937 mt(rd());//non deterministic random seed
	std::uniform_int_distribution<T> random(start, end);
	return random(mt);
}


template<typename T>
T randomnumgenfloat(T start, T end) {
	std::random_device rd;
	std::mt19937 mt(rd());//non deterministic random seed
	std::uniform_real_distribution<T> random(start, end);
	return random(mt);
}

ThroneRoomTeleporterScript::ThroneRoomTeleporterScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void ThroneRoomTeleporterScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	audio.changeBGM(audio.bgmThroneRoom);
	state = scriptBase::ScriptState::UPDATE;

	//create EmitterComponent
	EmitterComponent EC{};
	EC.emitterContainer.insert(std::make_pair("EmitterTeleporterTrail", std::make_shared<EmitterTeleporterTrail>()));
	ecs.addComponent<EmitterComponent>(id, EC);
}

void ThroneRoomTeleporterScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
	entityID playerEntity{ 1 }; //HACK
	//mouse picking 
	if (ecs.entityHas<transform2D>(id)) {
		auto& oTransform = ecs.getComponent<transform2D>(id);
		auto& eInnerTransform = ecs.getComponent<transform2D>(playerEntity);
		auto& oSprite = ecs.getComponent<spriteCom>(id);//getting sprite 

		
			if (eInnerTransform.pos.x > (oTransform.pos.x - radiusTeleport) && eInnerTransform.pos.x < (oTransform.pos.x + radiusTeleport) &&
				eInnerTransform.pos.y >(oTransform.pos.y - radiusTeleport) && eInnerTransform.pos.y < (oTransform.pos.y + radiusTeleport)) {
				//SoundClass::getInstance()->channels[Teleporter]->setPaused(false);
				
				emitterPos = oTransform.pos;
				emitterPos.x = randomnumgenfloat(oTransform.pos.x - emitterRangeX, oTransform.pos.x + emitterRangeX);
				emitterPos.y = randomnumgenfloat(oTransform.pos.y, oTransform.pos.y + emitterRangeY);

				//particle for teleporter 
				if (ecs.entityHas<EmitterComponent>(id))
				{
					EmitterTeleporterTrail* emi = dynamic_cast<EmitterTeleporterTrail*>(ecs.getComponent<EmitterComponent>(id).emitterContainer["EmitterTeleporterTrail"].get());
					if (emi)
						emi->setTriggered(true, emitterVelocity, emitterPos);
				}

				oSprite.textureId = "newTeleporter.png";

				//if text is loaded and the boss is dead
				if (KeyManager.isKeyTriggered('f', true))
				{
					//SoundClass::getInstance()->channels[throneRoom]->setPaused(true);
					ecs.loadLevel("Resources/SavedLevels/TeleporterMenuLevel.json");
					LogicSystemsPtr->sceneIsChanging = true;//it is changing scenes
				}
			}
			else {
				//SoundClass::getInstance()->channels[Teleporter]->setPaused(true);
				
				oSprite.textureId = "teleporterNotActive.png";
			}
		
	}
}

void ThroneRoomTeleporterScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> ThroneRoomTeleporterScript::createCopy()
{
	return std::make_shared <ThroneRoomTeleporterScript>();
}

std::string ThroneRoomTeleporterScript::getScriptName()
{
	return name;
}
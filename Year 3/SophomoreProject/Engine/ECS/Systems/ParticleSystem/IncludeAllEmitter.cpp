#include "IncludeAllEmitter.h"
#include "ParticleSystem.h"

//Emitter
#include "EmitterExplosion.h"
#include "EmitterExplosionDamaged.h"
#include "EmitterSuction.h"
#include "EmitterWalkingTrail.h"
#include "EmitterTeleporter.h"

void IncludeAllEmitter()
{
	EmitterManager.addEmitter(std::make_shared<EmitterExplosion>());
	EmitterManager.addEmitter(std::make_shared<EmitterExplosionDamaged>());
	EmitterManager.addEmitter(std::make_shared<EmitterSuction>());
	EmitterManager.addEmitter(std::make_shared<EmitterWalkingTrail>());
	EmitterManager.addEmitter(std::make_shared<EmitterTeleporterTrail>());
}

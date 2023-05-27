
#include "CharacterScript.h"

namespace TH
{
	using namespace BE;
	LevelManager* CharacterScript::cLevelMgr = nullptr;

	void CharacterScript::Awake()
	{	
		cModelID = id;
	}
	void CharacterScript::Init()
	{		
		if (cHealth_total < 0.f) cHealth_total = -cHealth_total;
		cHealth_current = cHealth_total;
		cTarget = NULL_ENTITY;
		cSpawn_Position = ECS->GetComponent<BE::Transform>(id).GetPosition();
		cSpawn_Rotation = ECS->GetComponent<BE::Transform>(id).GetRotation();
		
		//ECS->GetComponent<Renderer>(cModelID).setFlickerColor({ 1.f, 1.f, 1.f, 0.f});
		//ECS->GetComponent<Renderer>(cModelID).setFlickerDuration(0.15f);
	}
	
	void CharacterScript::Update(double dt)
	{
		// deny collider flipping over
		Vec3f eulerangle = ECS->GetComponent<BE::Transform>(id).GetEulerRotation();
		if (Math::Abs(eulerangle.x) > 0.1f || Math::Abs(eulerangle.z) > 0.1f) {
			eulerangle.x = 0.f; eulerangle.z = 0.f;
			ECS->GetComponent<BE::Transform>(id).SetEulerRotation(Vec3f(eulerangle.x, eulerangle.y, eulerangle.z));
		}

		if (IsDead() && id != GLS->GetPlayerID()) {
			if (cDestroy_Duration == 0.f) 
			{
				if(canDestroy) ECS->DestroyEntity(id);

				Vec3f tempPos = ECS->GetComponent<BE::Transform>(id).GetPosition() - Vec3f(0, 100, 0);
				ECS->GetComponent<BE::Transform>(id).SetPosition(tempPos);
				BE::ECS->GetComponent<BE::Physics>(id).SetIsTrigger(true);
				ECS->GetComponent<BE::EntityInfo>(id).SetFlag(BE::EntityFlag::active, false);
				cTookDamageFrame = false;
			}
			else if (destroyTimer < cDestroy_Duration) 
			{
				destroyTimer += static_cast<float>(dt);
				if (destroyTimer >= cDestroy_Duration) 
				{
					if (canDestroy) ECS->DestroyEntity(id);

					Vec3f tempPos = ECS->GetComponent<BE::Transform>(id).GetPosition() - Vec3f(0, 100, 0);
					ECS->GetComponent<BE::Transform>(id).SetPosition(tempPos);
					BE::ECS->GetComponent<BE::Physics>(id).SetIsTrigger(true);
					ECS->GetComponent<BE::EntityInfo>(id).SetFlag(BE::EntityFlag::active, false);
					cTookDamageFrame = false;

					destroyTimer = 0.f;
				}
			}
			return;
		}

		cStatus.UpdateStatusTimer(static_cast<float>(dt));
	}	


}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<CharacterScript>(RegistrationNameCreator<CharacterScript>())
		.constructor()
		.property("cHealth_total", &CharacterScript::cHealth_total)
		.property("cHealth_current", &CharacterScript::cHealth_current)
		.property("cSpeed_Base", &CharacterScript::cSpeed_Base)
		.property("cAttack_Base", &CharacterScript::cAttack_Base)
		.property("KnockBack Resistance", &CharacterScript::cKB_resistance)
		.property("Destroy Delay", &CharacterScript::cDestroy_Duration)
		.property("Aggro Range", &CharacterScript::cAggro_range)
		.property("Invulnerable", &CharacterScript::cInvulnerable);
}
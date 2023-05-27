#pragma once
#ifndef	CHARACTER_SCRIPT_H
#define CHARACTER_SCRIPT_H

#include <BananaEngine.h>
#include "BaseSkill.h"
#include "VirtualCharacterScript.h"
#include "../Level/LevelManager.h"

namespace TH
{
	using namespace BE;

	// helper function
	template <typename T>
	inline void GetOrAddOtherRequiredScripts(T*& pointer_to_script, BEID id)
	{
		auto script_ptr = ECS->GetComponent<GameLogic>(id).GetScript<T>();
		if (script_ptr.get() == nullptr) {
			ECS->GetComponent<GameLogic>(id).AddScript<T>();
			script_ptr = ECS->GetComponent<GameLogic>(id).GetScript<T>();
		}
		pointer_to_script = dynamic_cast<T*>(script_ptr.get());
	}

	struct Status
	{
		StatusType status_type = StatusType::NoStatus;
		float duration = 0.f;
		float timer = 0.f;

		inline void UpdateStatusTimer(float dt) {
			if (status_type != StatusType::NoStatus) {
				timer += dt;
				if (timer >= duration) {
					timer = 0.f;
					status_type = StatusType::NoStatus;
				}
			}
		}
	};

	struct AnimInfo
	{
		int index = -1;
		float duration = 0.f;
	};

	class CharacterScript : public IScript
	{
		Vec3f cSpawn_Position;
		Quatf cSpawn_Rotation;
		float cHealth_total = 100.f;
		float cHealth_current = 100.f;
		float cSpeed_Base = 3.0f;
		float cAttack_Base = 10.f;
		float cReset_range = 20.0f;
		float cAggro_range = 4.0f;
		float cKB_resistance = 0.0f;

		float cDestroy_Duration = 0.01f;
		float destroyTimer = 0.f;

		EntityID cTarget = NULL_ENTITY;
		EntityID cModelID = NULL_ENTITY;
		bool cInvulnerable = false;
		bool cIsVirtual = false;
		bool cKBImmune = false;
		
		Status cStatus;
		//Vec3f scale;
	public:
		bool cTookDamageFrame = false;
		bool canDestroy = false;
		static LevelManager* cLevelMgr;

		void Awake();
		void Init();

		void Update(double);

		// Setters
		inline void SetModelID(EntityID _id) { cModelID = _id; }
		inline void SetDestroyDuration(float in) { cDestroy_Duration = in; }
		inline void SetIsVirtual (bool _in) { cIsVirtual = _in; }
		inline void SetKnockBackImmune (bool _in) { cKBImmune = _in; }
		inline void SetTotalHealth(float hp) { cHealth_total = hp; }
		inline void SetCurrentHealth(float hp) { cHealth_current = hp; }
		inline void SetResetRange(float range) { cReset_range = range; }
		inline void SetAggroRange(float range) { cAggro_range = range; }
		inline void SetKnockBackResistance(float resist) { cKB_resistance = resist; }
		inline void DamageHealth(float damage)
		{
			if (cInvulnerable || IsDead()) return; cHealth_current -= damage; if (cHealth_current < 0.f) cHealth_current = 0.f;
			ECS->GetComponent<Renderer>(cModelID).setShouldFlicker(true);
			cTookDamageFrame = true;

			if (id == GLS->GetPlayerID()) {
				auto pair = BE::UI->FindUIElementByName("HPBar", 1);
				float percent = cHealth_current / cHealth_total;
				BE::UI->GetCanvas(pair[0].first)->GetUI<BE::UIProgressBar>(pair[0].second)->SetPercentage(percent);
			}
			
			if (damage <= 0.0f)
				return;

			if (cHealth_current == 0.f && cLevelMgr)
			{
				if (BE::ECS->GetComponent<BE::EntityInfo>(id).HasTag("Player"))
					BE::ECS->GetComponent<BE::AudioSource>(id).Play(BE::AssetID(10210675003471560138));			//TeeheeDeath.wav

				cLevelMgr->CharacterDied(id);
			}
			else
			{
				if (!BE::ECS->GetComponent<BE::EntityInfo>(id).HasTag("Player"))
					return;


				if(BE::Rand::Random<int>(0,1) == 0)
					BE::ECS->GetComponent<BE::AudioSource>(id).Play(BE::AssetID(10210675003471560136));		//HitSound1.wav
				else
					BE::ECS->GetComponent<BE::AudioSource>(id).Play(BE::AssetID(10210675003471560137));		//HitSound2.wav
			}
		}
		inline void RecoverHealth(float heal) { cHealth_current += heal; if (cHealth_current > cHealth_total) cHealth_current = cHealth_total; }
		inline void SetBaseSpeed(float _speed) { cSpeed_Base = _speed; }
		inline void SetBaseAttack(float _attack) { cAttack_Base = _attack; }
		inline void SetTarget(EntityID _target) { cTarget = _target; }
		inline void SetInvulnerable(bool set) { cInvulnerable = set; }
		inline void KillCharacter() { DamageHealth(cHealth_current); }
		inline void InflictStatus(StatusType status, float duration) 
		{ 
			if (cStatus.status_type != StatusType::NoStatus || status == StatusType::NoStatus) return;
			cStatus.status_type = status; cStatus.duration = duration; cStatus.timer = 0.f;
		}
		inline void KnockBackCharacter(Vec3f const& dir_unNormalised, float knockback_factor)
		{
			if (cInvulnerable || IsDead() || cKBImmune) return;
			float kb = knockback_factor - cKB_resistance;
			Vec3f dir = dir_unNormalised; 
			dir.y = 0.f;
			if (kb > 0) {
				if(!cIsVirtual) ECS->GetComponent<Physics>(id).SetLinearVelocity(ECS->GetComponent<Physics>(id).GetLinearVelocity() + dir.Normalised() * kb);
				else {
					auto scrpt_ptr = ECS->GetComponent<GameLogic>(id).GetScript<VirtualCharacter>();
					VirtualCharacter* char_ptr = dynamic_cast<VirtualCharacter*>(scrpt_ptr.get());
					if (char_ptr) {
						char_ptr->PushCharacter(dir.Normalised() * kb);
					}
				}
			}
		}
		// Getters
		inline EntityID GetModelID() const { return cModelID; }
		inline float GetTotalHealth() const {return cHealth_total;}
		inline float GetCurrentHealth() const { return cHealth_current; }
		inline float GetHealthPercentage() const { return cHealth_current / cHealth_total; }
		inline float GetBaseSpeed() const { return cSpeed_Base; }
		inline float GetBaseAttack() const { return cAttack_Base; }
		inline float GetResetRange() const { return cReset_range; }
		inline float GetAggroRange() const { return cAggro_range; }
		inline EntityID GetTarget() const { return cTarget; }
		inline bool IsAlive() const {return cHealth_current > 0.f; }
		inline bool IsDead() const {return cHealth_current <= 0.f; }
		inline bool HasTarget() const {return cTarget != NULL_ENTITY; }
		inline bool NoTarget() const {return cTarget == NULL_ENTITY; }
		inline bool IsInvulnerable() const {return cInvulnerable; }
		inline bool IsKnockbackImmune() const {return cKBImmune; }
		inline bool IsStunned() const {return cStatus.status_type == StatusType::Stun; }
		inline Vec3f GetSpawnPos() const { return cSpawn_Position; }
		inline Quatf GetSpawnRot() const { return cSpawn_Rotation; }
		inline StatusType GetStatusType() const { return cStatus.status_type; }
		inline Status const& GetStatus() const { return cStatus; }

		inline Vec3f GetCurrentDirection() {
			Mat3f rotMatrix;
			rotMatrix.SetRotation(ECS->GetComponent<Transform>(id).GetRotation());
			return rotMatrix * Vec3f::Forward();
		}
		inline Vec3f GetFrontalPos(EntityID model_id, float forward_dist = 0.f )
		{
			Mat3f rotMatrix;
			rotMatrix.SetRotation(ECS->GetComponent<Transform>(id).GetRotation());
			Vec3f frontal = ECS->GetComponent<Transform>(model_id).GetPosition() + rotMatrix * (Vec3f::Forward() * forward_dist);
			return frontal;
		}
		inline Vec3f GetBottomPos(EntityID model_id, float downward_dist = 0.f)
		{
			Mat3f rotMatrix;
			rotMatrix.SetRotation(ECS->GetComponent<Transform>(id).GetRotation());
			Vec3f bottom = ECS->GetComponent<Transform>(model_id).GetPosition() + rotMatrix * (Vec3f::Down() * downward_dist);
			return bottom;
		}
		inline Vec3f GetNormXZDirectionToPosition(Vec3f const& in)
		{
			Vec3f dir = in - ECS->GetComponent<Transform>(id).GetPosition();
			dir.y = 0.f;
			return dir.Normalised();
		}
		inline Vec3f GetNormXZDirectionToEntity(EntityID in)
		{
			return GetNormXZDirectionToPosition(ECS->GetComponent<Transform>(in).GetPosition());
		}

		inline bool IsGrounded(EntityID model_id, float scale, Vec3f& ground_normal)
		{
			Vec3f temp_contact;
			Mat3f rotMatrix;
			rotMatrix.SetRotation(ECS->GetComponent<Transform>(id).GetRotation());
			Vec3f down_dir = rotMatrix * PhySys->GetGravity().Normalise() * 0.01f;
			Vec3f bottom = GetBottomPos(model_id, scale + 0.01f);
			return NULL_ENTITY != PhySys->RayCastSingleHit(temp_contact, ground_normal, bottom
				, down_dir);
		}

		inline bool GetLOStoEntity(EntityID target)
		{
			std::vector<EntityID> entities_in_sight;
			std::vector<Vec3f> contacts;
			Vec3f pos = ECS->GetComponent<Transform>(id).GetPosition();
			Vec3f ppos = ECS->GetComponent<Transform>(target).GetPosition();
			PhySys->RayCast(entities_in_sight, contacts, pos
				, ppos - pos, EntityTagFilter({  }, FilterType::NoFilter, 0, id));

			return entities_in_sight.empty();
		}

		inline void RespawnPosition() {
			ECS->GetComponent<EntityInfo>(id).SetFlag(EntityFlag::active, true);
			Vec3f respawnPoint = GetSpawnPos();
			ECS->GetComponent<Transform>(id).SetPosition(respawnPoint);
			ECS->GetComponent<Physics>(id).SetIsTrigger(false);
		}
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};


}

#endif
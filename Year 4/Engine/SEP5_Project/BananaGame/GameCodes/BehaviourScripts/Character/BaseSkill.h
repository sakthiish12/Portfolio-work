#pragma once
#ifndef	BASE_SKILL_H
#define BASE_SKILL_H

#include <BananaEngine.h>
#include "../../Application/GameConfig.h"
#include "../Misc/Timers.h"

namespace TH
{
	using namespace BE;

	struct BaseSkill
	{
		CoolDown sCooldown;
		float sDamage_Multplr = 1.0f;
		float sCast_Range = 1.f;
		float sKnockBack = 10.0f;
		float sAllyResistance = 0.f;

		inline bool IsOnCoolDown() { return sCooldown.IsOnCoolDown(); }
		inline void UpdateCoolDown(float dt) { sCooldown.update(dt); }

		BaseSkill() = default;
		BaseSkill(float CD_duration, float Damage_Multplr, float Cast_Range = 1.f) :
			sCooldown{ CoolDown(CD_duration) }, sDamage_Multplr{ Damage_Multplr }, sCast_Range{ Cast_Range }
		{
		}

		inline float GetCoolDownDuration() { return sCooldown.duration; }
		inline void SetCoolDownDuration(float duration) { sCooldown.duration = duration; }
	};

	enum class StatusType
	{
		NoStatus,
		Stun,
		Burn
	};

	class Projectile : public IScript
	{
		EntityID firer = NULL_ENTITY;
		AssetID hitSFX = NULL_ASSET;
		std::string toIgnore;
		float status_duration;
		float pDamage_Final = 1.f;
		bool CanDestroy = false;
		bool DestroyOnHitAny = true;
		bool IsCharging = false;
		StatusType status_effect = StatusType::NoStatus;
	public:

		void Update(double);
		void OnCollision(EntityID collidingID);

		inline void SetFinalDamage(float dmg) { pDamage_Final = dmg; }
		inline void SetFirerID(EntityID firer_id) { firer = firer_id; }
		inline void SetStatusEffect(StatusType effect) { status_effect = effect; }
		inline void SetStatusDuration(float _status_duration) { status_duration = _status_duration; }
		inline void SetDestroyOnHitAny(bool in) { DestroyOnHitAny = in; }
		inline void SetTagtoIgnore(std::string const& in) { toIgnore = in; }
		inline void SetIsCharging(bool in) { IsCharging = in; }

		inline void SetAll(float dmg, EntityID firer_id, StatusType effect, float _status_duration, bool DestroyOnhit, std::string const& tagtoignore, bool isCharged, AssetID sfxID)
		{
			pDamage_Final = dmg;
			firer = firer_id;
			status_effect = effect;
			status_duration = _status_duration;
			DestroyOnHitAny = DestroyOnhit;
			toIgnore = tagtoignore;
			IsCharging = isCharged;
			hitSFX = sfxID;
		}


		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	struct RayCastSkill : public BaseSkill
	{
		Charge rCharge = Charge(0.5f);
		float rRadius = 0.01f;
		float rSpeed = 10.f;
		float rRayLength = 100.f;

		enum class RayType {
			LineRay,
			LineRayPierce,
		} rRayType;

		RayCastSkill() = default;
		RayCastSkill(float CD_duration, float Damage_Multplr, float Charge_duration, float Speed, RayType rayType, float RayLength = 100.f, float Cast_Range = 1.f, float Radius = 0.01f ) :
			BaseSkill{ CD_duration, Damage_Multplr, Cast_Range }, rCharge{ Charge(Charge_duration) }, rRadius{ Radius }, rSpeed{ Speed }, rRayLength{ RayLength }
		{
			rRayType = rayType;
		}
		
		void CastSkill(EntityID attacker, Vec3f origin_of_cast, Vec3f dir, float base_attack, std::vector<Vec3f>& contacts, EntityTagFilter const& filter = {});
	};

	struct ProjectileSkill : public BaseSkill
	{
		std::string pName = "Projectile";
		std::string pIgnoreTag;
		float pRadius = 0.01f;
		float pSpeed = 10.f;
		uint8_t pBurst_amount = 1;
		StatusType pStatus_effect;
		float pStatus_duration = 2.f;
		Shape pShape = Shape::Sphere;
		AssetID pPrefabID = NULL_ASSET;

		enum class ProjectileType {
			Straight,
			Weighted
		} pProjectile_type;

		bool pDestroyOnHitAny = true;

		ProjectileSkill() = default;
		ProjectileSkill(float CD_duration, float Damage_Multplr, float Radius, float Speed, ProjectileType Projectile_type, std::string const& projectile_name = {},
			float Cast_Range = 1.f, StatusType effect = StatusType::NoStatus, float _status_duration = 2.f, bool DestroyOnHitAny = true) :
			BaseSkill{ CD_duration, Damage_Multplr, Cast_Range }, pName{ projectile_name }, pRadius{ Radius }, pSpeed{ Speed }
		{
			pProjectile_type = Projectile_type;
			pStatus_effect = effect;
			pStatus_duration = _status_duration;
			pDestroyOnHitAny = DestroyOnHitAny;
		}

		inline void SetProjectileShape(Shape in) { pShape = in; }

		inline EntityID CreateProjectile(Vec3f const& spawn_pos, Quatf const& rotation, EntityID attacker, float base_attack, bool isCharged = false);

		void CastSkill_Direction(Vec3f const& origin, Vec3f const& dir, EntityID attacker, float base_attack);

		void CastSkill_EndPoint(Vec3f const& origin, Vec3f const& end_point, EntityID attacker, float base_attack);
	};

	struct AttackSkill : public BaseSkill
	{
		Vec3f aAOE_size = { 1.0f, 1.f ,1.f };

		enum class AOEType {
			Single,
			AABox,
			Sphere
		};
		AOEType aAOE_type = AOEType::Single;

		AttackSkill() = default;
		AttackSkill(float CD_duration, float Damage_Multplr, Vec3f const& AOE_size, AOEType aoe_type, float Cast_Range = 1.f) :
			BaseSkill{ CD_duration, Damage_Multplr, Cast_Range}, aAOE_size{ AOE_size }
		{
			aAOE_type = aoe_type;
		}

		void CastSkill(EntityID attacker, Vec3f origin_of_cast, float base_attack, std::vector<Vec3f>& contacts, EntityID single_target = NULL_ENTITY, EntityTagFilter const& filter = {});
		RTTR_ENABLE()
	};


	struct ChargedProjectile : public ProjectileSkill
	{
		float cCharge_timer = 0.f;
		float cCharge_duration = 1.f;
		float cCharge_size_multiplr = 2.0f;
		EntityID cProjectile_id = NULL_ENTITY;
		//EntityID cTarget = NULL_ENTITY;

		ChargedProjectile() = default;
		ChargedProjectile(float CD_duration, float Damage_Multplr, float Radius, float Speed, ProjectileType Projectile_type, std::string const& projectile_name = {},
			float Cast_Range = 1.f, StatusType effect = StatusType::NoStatus, float _status_duration = 2.f, bool DestroyOnHitAny = true) :
			ProjectileSkill(CD_duration, Damage_Multplr, Radius, Speed, Projectile_type, projectile_name, Cast_Range, effect, _status_duration, DestroyOnHitAny)
		{}

		void ChargeAndCast(Vec3f const& origin, Quatf const& rotation, EntityID attacker, float base_attack);
		// returns true if projectile fired.
		bool UpdateCharging(Vec3f const& dir, float dt, bool isCancelled = false);
		inline bool IsCharging() const { return cProjectile_id != NULL_ENTITY; }
		inline void ResetCharges() {
			cCharge_timer = 0.f;
			cProjectile_id = NULL_ENTITY;
			//cTarget = NULL_ENTITY;
		}
		inline void CancelCharge() {
			if(ECS->DoesEntityExists(cProjectile_id))
				ECS->DestroyEntity(cProjectile_id);
			ResetCharges();
		}

		inline ProjectileSkill const& GetBase() { return *dynamic_cast<ProjectileSkill*>(this); }
		inline void SetBase(ProjectileSkill const& a) { *dynamic_cast<ProjectileSkill*>(this) = a; }
		RTTR_ENABLE()
	};


	class Beam : public IScript
	{
		std::string IgnoreTag;
		EntityID firer = NULL_ENTITY;
		AssetID hitSFX = NULL_ASSET;
		float pDamage_Final = 1.f;
		float KnockBack = 10.f;
		bool enabled = true;

		inline void DamageTarget(BEID target, bool isvirtual = false);
	public:

		void OnPersistedCollision(BEID collidingID);
		void OnCollision(BEID collidingID) override;
		inline void SetFinalDamage(float dmg) { pDamage_Final = dmg; }
		inline void SetFirerID(EntityID firer_id) { firer = firer_id; }
		inline void Enable(bool in) { enabled = in; }
		inline void SetIgnoreTag(std::string const& in) { IgnoreTag = in; }

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};


	struct ChargeBeamSkill : public BaseSkill
	{
		Charge cCharge = Charge(1.5f);
		Vec3f cLocalPosition = Vec3f();
		float cCharge_size_multiplr = 0.5f;
		float cRadius = 0.1f;
		float cSpeed = 10.f;
		float cRayLength = 100.f;
		float cBlast_duration = 3.f;
		float cBlast_timer = 0.f;
		EntityID cBeamBlast = NULL_ENTITY;
		EntityID cBeamSource = NULL_ENTITY;
		bool IsBlasting = false;
		bool IsExtending = false;
		bool CanCloseBeam = false;
		bool endCharge = false;
		bool IsChargeStart = false;

		ChargeBeamSkill() = default;
		ChargeBeamSkill(float CD_duration, float Damage_Multplr, float Charge_duration, float Speed, float RayLength = 100.f, float Cast_Range = 1.f, float Radius = 0.1f) :
			BaseSkill{ CD_duration, Damage_Multplr, Cast_Range }, cCharge{ Charge(Charge_duration) }, cRadius{ Radius }, cSpeed{ Speed }, cRayLength{ RayLength }
		{
		}
		void InitChargeBeam(EntityID attacker, Vec3f const& local_origin, EntityID beamBlast, EntityID beamSource, float base_attack, std::string const& ignoreTagtarget= "");
		void StartCharge();

		void UpdateCharging(float dt);
		bool UpdateBlasting(Vec3f const& dir_length, float dt, bool useBabySFX = false);
		void UpdateEndCharge(Vec3f const& dir_length, float dt);

		void CancelCharge();

		inline bool IsReadyToEndCharge() {
			return !IsBlasting && !IsExtending && ECS->GetComponent<EntityInfo>(cBeamSource).HasFlag(EntityFlag::active) && !cCharge.IsCharging();
		}
		inline bool IsFiring() const {
			return (IsBlasting || IsExtending);
		}
		inline bool IsInAnimation() const {
			return IsBlasting || IsExtending || CanCloseBeam;
		}
	};

	
}
#endif
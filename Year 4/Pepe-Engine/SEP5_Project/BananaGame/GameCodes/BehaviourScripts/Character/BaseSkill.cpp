
#include "BaseSkill.h"
#include "CharacterScript.h"
#include <Misc/SFXPlayer.h>
#include <Enemies/EnemyScript.h>
namespace TH
{
	using namespace BE;
	static const AssetID laserFireSFX = AssetID(2773954843876887764);
	static const AssetID laserFire2SFX = AssetID(10314355125321005166);
	static const std::array< AssetID, 3> enemyHitSFX
	{
		 AssetID(6678469896061799648),
		 AssetID(2470535914051851891),
		 AssetID(7859595392513741910)
	};
	void Projectile::Update(double) {
		if (CanDestroy || !Within_World_Bounds(ECS->GetComponent<Transform>(id).GetPosition())) {
			if (hitSFX)
			{
				EntityID sfxPlayerId = ECS->CreateEntity();
				ECS->AddComponent<GameLogic>(sfxPlayerId);
				ECS->GetComponent<GameLogic>(sfxPlayerId).AddScript<SFXPlayer>();
				static_cast<SFXPlayer&>(*ECS->GetComponent<GameLogic>(sfxPlayerId).GetScript<SFXPlayer>()).Setup(hitSFX, 1.0f, 0.5f);
			}
			ECS->DestroyEntity(id);
		}
	}

	void Projectile::OnCollision(EntityID collidingID)
	{
		//std::cout << ECS->GetComponent<EntityInfo>(collidingID).GetName() << " with ID: " << collidingID << " Firer ID: " << firer << "\n";

		// skip if the projectile is colliding with the firer
		if (collidingID == firer || IsCharging)
			return;
		bool trigger = ECS->GetComponent<Physics>(collidingID).GetIsTrigger();

		CharacterScript* char_ptr = nullptr;
		if (ECS->HasComponent<GameLogic>(collidingID)) {
			auto scrpt_ptr = ECS->GetComponent<GameLogic>(collidingID).GetScript<CharacterScript>();
			char_ptr = dynamic_cast<CharacterScript*>(scrpt_ptr.get());
		}

		if (trigger && char_ptr == nullptr) {
			return;
		}

		// skip tagged to ignore
		if (toIgnore.size() &&
			ECS->GetComponent<EntityInfo>(collidingID).HasTag(toIgnore))
			return;

		if (DestroyOnHitAny) CanDestroy = true;
		if (ECS->HasComponent<GameLogic>(collidingID)) {

			if (char_ptr != nullptr) {
				char_ptr->DamageHealth(pDamage_Final);
				char_ptr->InflictStatus(status_effect, status_duration);
				CanDestroy = true;
			}
		}
	}

	void RayCastSkill::CastSkill(EntityID attacker, Vec3f origin_of_cast, Vec3f dir, float base_attack, std::vector<Vec3f>& contacts, EntityTagFilter const& filter)
	{
		sCooldown.start = true;
		std::vector<EntityID> characters_in_AOE;
		Vec3f single_contact, single_normal;
		contacts.clear();

		Vec3f dirNorm = dir.Normalised() * rRayLength;

		switch (rRayType) {
		case RayType::LineRay: {
			EntityID hit_id = PhySys->RayCastSingleHit(single_contact, single_normal, origin_of_cast, dirNorm, filter);
			if (hit_id != NULL_ENTITY) {
				characters_in_AOE.emplace_back(hit_id);
				contacts.emplace_back(single_contact);
			}
			break;
		}
		case RayType::LineRayPierce: {
			PhySys->RayCast(characters_in_AOE, contacts, origin_of_cast, dirNorm, filter);
			break;
		}
		}

		for (EntityID id : characters_in_AOE) {
			if (attacker == id || !ECS->HasComponent<GameLogic>(id)) continue;
			auto scrpt_ptr = ECS->GetComponent<GameLogic>(id).GetScript<CharacterScript>();
			CharacterScript* char_ptr = dynamic_cast<CharacterScript*>(scrpt_ptr.get());
			if (char_ptr) {
				char_ptr->DamageHealth(base_attack * sDamage_Multplr);
			}
		}
	}
	inline AssetID SetProjectileAudio(AssetID id)
	{
		switch (id)
		{
		case 5999381398450237735:
			return AssetID(6628869859723502890);//BananaSplat.wav
			break;
		default:
			return NULL_ASSET;
		}
	}
	inline EntityID ProjectileSkill::CreateProjectile(Vec3f const& spawn_pos, Quatf const& rotation, EntityID attacker, float base_attack, bool isCharged)
	{
		EntityID pid;
		if (pPrefabID == NULL_ASSET) {
			pid = ECS->CreateEntity();
			Transform& transf = ECS->GetComponent<Transform>(pid);
			transf.SetPosition(spawn_pos); transf.SetScale({ pRadius, pRadius, pRadius });
			transf.SetRotation(rotation);
			ECS->GetComponent<EntityInfo>(pid).SetName(std::string(pName));

			// Add physics to projectile
			ECS->AddComponent<Physics>(pid, Physics(pShape, pRadius, pRadius, pRadius));
			Physics& physics = ECS->GetComponent<Physics>(pid);
			if (pProjectile_type == ProjectileType::Straight) physics.SetMotionType(JPH::EMotionType::Kinematic);
			else physics.SetMotionType(JPH::EMotionType::Dynamic);
			physics.SetIsTrigger(pDestroyOnHitAny);
			physics.SetCanPushCharacter(false);
			physics.SetCanReceiveCharacterImpulse(false);
			physics.SetAngularDamping(1.f);
			physics.SetMaxAngularVelocity(0.1f);
			physics.SetFriction(10.0f);

			// Add renderer to projectile
			ECS->AddComponent<Renderer>(pid, Renderer());
			Renderer& renderer = ECS->GetComponent<Renderer>(pid);
			if (pShape == Shape::Box) renderer.setModelIDwithName("../Assets/Models/Primitives/Cube.bananaModel");
			else renderer.setModelIDwithName("../Assets/Models/Primitives/Sphere.bananaModel");
			renderer.setAlbedoAlphaMapwithName("../Assets/Textures/misc/yellow.dds", 0);
			renderer.setEmissiveMapwithName("../Assets/Textures/misc/yellow.dds", 0);
			// Add Projectile script to projectile
			ECS->AddComponent<GameLogic>(pid);
		}
		else {
			pid = ECS->SpawnPrefab(pPrefabID);
			Transform& transf = ECS->GetComponent<Transform>(pid);
			transf.SetPosition(spawn_pos);
			transf.SetScale(pRadius);
			transf.SetRotation(rotation);
			if (ECS->HasComponent<Physics>(pid)) ECS->GetComponent<Physics>(pid).SetShapeSize({ pRadius, pRadius, pRadius });
		}

		//ECS->GetComponent<GameLogic>(pid).AddScript<Projectile>();
		auto scrpt_ptr = ECS->GetComponent<GameLogic>(pid).GetScript<Projectile>();
		if (scrpt_ptr == nullptr) {
			ECS->GetComponent<GameLogic>(pid).AddScript<Projectile>();
			scrpt_ptr = ECS->GetComponent<GameLogic>(pid).GetScript<Projectile>();
		}
		Projectile* p_ptr = dynamic_cast<Projectile*>(scrpt_ptr.get());
		if (p_ptr) {
			// Set all the data that the projectile script needs
			p_ptr->SetAll(base_attack * sDamage_Multplr,
				attacker,
				pStatus_effect,
				pStatus_duration,
				pDestroyOnHitAny,
				pIgnoreTag,
				isCharged,
				SetProjectileAudio(pPrefabID));
		}
		return pid;
	}

	void ProjectileSkill::CastSkill_Direction(Vec3f const& origin, Vec3f const& dir, EntityID attacker, float base_attack)
	{
		sCooldown.start = true;
		EntityID pid = CreateProjectile(origin, Vec3f::Forward().get_rotation_between(dir), attacker, base_attack);
		Physics& phys = ECS->GetComponent<Physics>(pid);
		Vec3f vel = dir.Normalised() * pSpeed;
		phys.SetLinearVelocity(vel);
	}

	void ProjectileSkill::CastSkill_EndPoint(Vec3f const& origin, Vec3f const& end_point, EntityID attacker, float base_attack)
	{
		sCooldown.start = true;

		EntityID pid = CreateProjectile(origin, Vec3f::Forward().get_rotation_between(end_point - origin), attacker, base_attack);
		Physics& phys = ECS->GetComponent<Physics>(pid);
		if (pProjectile_type == ProjectileType::Straight) {
			phys.SetLinearVelocity((end_point - origin).Normalised() * pSpeed);
		}
		else {
			float ydiff = end_point.y - origin.y;
			float tan45 = 1.f;
			float gravity = PhySys->GetGravity().y;
			float dist_xz = origin.XZ_DistBetween(end_point);

			float VelY = sqrtf((0.5f * gravity * dist_xz * dist_xz * tan45 * tan45) / (ydiff - dist_xz * tan45));
			float VelX = ((end_point.x - origin.x) / dist_xz) * VelY;
			float VelZ = ((end_point.z - origin.z) / dist_xz) * VelY;

			phys.SetLinearVelocity({ VelX, VelY, VelZ });
		}
	}

	void AttackSkill::CastSkill(EntityID attacker, Vec3f origin_of_cast, float base_attack, std::vector<Vec3f>& contacts, EntityID single_target, EntityTagFilter const& filter)
	{
		sCooldown.start = true;
		std::vector<EntityID> characters_in_AOE;
		Vec3f single_contact, single_normal;
		contacts.clear();

		switch (aAOE_type) {
		case AOEType::Single: {
			if (single_target != NULL_ENTITY) {
				characters_in_AOE.emplace_back(single_target);
			}
			break;
		}
		case AOEType::AABox: {
			Vec3f min = origin_of_cast - aAOE_size;
			Vec3f max = origin_of_cast + aAOE_size;
			PhySys->AABoxCollide(characters_in_AOE, min, max, filter.tags, filter.filter_type, filter.OnlyTrigger);
			break;
		}
		case AOEType::Sphere: {
			PhySys->SphereCollide(characters_in_AOE, origin_of_cast, aAOE_size.x, filter.tags, filter.filter_type, filter.OnlyTrigger);
			break;
		}
		}

		bool hitAudio = false;
		for (EntityID id : characters_in_AOE) {
			if (attacker == id || !ECS->HasComponent<GameLogic>(id)) continue;
			auto scrpt_ptr = ECS->GetComponent<GameLogic>(id).GetScript<CharacterScript>();
			CharacterScript* char_ptr = dynamic_cast<CharacterScript*>(scrpt_ptr.get());
			if (char_ptr) {
				Vec3f hitpos = ECS->GetComponent<Transform>(id).GetPosition();

				if (ECS->GetComponent<EntityInfo>(id).HasTag("Friendly")) {
					char_ptr->DamageHealth(base_attack * sDamage_Multplr * (1 - sAllyResistance));
				}
				else char_ptr->DamageHealth(base_attack * sDamage_Multplr);

				char_ptr->KnockBackCharacter(hitpos - ECS->GetComponent<Transform>(attacker).GetPosition(), sKnockBack);
				contacts.emplace_back(hitpos);
				if (!hitAudio && GLS->GetPlayerID() == attacker && ECS->GetComponent<GameLogic>(id).HasScript<EnemyBase>())
				{
					std::cout << "Play robot hit sound\n";
					hitAudio = true;
					AudioSource& as = ECS->GetComponent<AudioSource>(id);
					unsigned random = Rand::Random(0u, static_cast<unsigned>(enemyHitSFX.size()) - 1u);
					as.Play(enemyHitSFX[random]);
				}
			}
		}
	}

	void ChargedProjectile::ChargeAndCast(Vec3f const& local_origin, Quatf const& rotation, EntityID attacker, float base_attack)
	{
		cProjectile_id = CreateProjectile(local_origin, rotation, attacker, base_attack, true);
		ECS->GetComponent<Transform>(cProjectile_id).AttachParent(attacker);
		ECS->GetComponent<Transform>(cProjectile_id).SetLocalPosition(local_origin);
		ECS->GetComponent<Transform>(cProjectile_id).SetLocalRotation(Quatf());
		ECS->GetComponent<Physics>(cProjectile_id).SetIsTrigger(true);
		//ECS->GetComponent<Physics>(cProjectile_id).SetMotionType(JPH::EMotionType::Kinematic);
		//cTarget = target;
	}

	bool ChargedProjectile::UpdateCharging(Vec3f const& dir, float dt, bool isCancelled)
	{
		// if a new projectile is created and timer is less than the duration.
		if (ECS->DoesEntityExists(cProjectile_id) && cCharge_timer < cCharge_duration)
		{
			// increase timer and size of projectile.
			cCharge_timer += dt;
			Vec3f scale = ECS->GetComponent<Transform>(cProjectile_id).GetScale();
			Vec3f scaleUpdated = Vec3f(scale + scale * (cCharge_size_multiplr - 1.f) * dt);
			ECS->GetComponent<Transform>(cProjectile_id).SetScale(scaleUpdated);

			// cancel charging if stunned or some other event that might cause it
			if (isCancelled) {
				ECS->DestroyEntity(cProjectile_id);
				ResetCharges();
			}
			// once charging is done, shoot the projectile out.
			if (cCharge_timer >= cCharge_duration) {
				sCooldown.start = true;
				ECS->GetComponent<Transform>(cProjectile_id).DetachParent();
				ECS->GetComponent<Physics>(cProjectile_id).SetShapeSize(scaleUpdated);
				ECS->GetComponent<Physics>(cProjectile_id).SetIsTrigger(false);
				Vec3f dir_norm = dir.Normalised();
				ECS->GetComponent<Physics>(cProjectile_id).SetLinearVelocity(dir_norm * pSpeed);

				auto scrpt_ptr = ECS->GetComponent<GameLogic>(cProjectile_id).GetScript<Projectile>();
				Projectile* p_ptr = dynamic_cast<Projectile*>(scrpt_ptr.get());
				if (p_ptr) {
					p_ptr->SetIsCharging(false);
				}
				ResetCharges();
				return true;
			}
		}

		return false;
	}


	void ChargeBeamSkill::InitChargeBeam(EntityID attacker, Vec3f const& local_origin, EntityID beamBlast, EntityID beamSource, float base_attack, std::string const& ignoreTagtarget)
	{
		cBeamBlast = beamBlast;
		cBeamSource = beamSource;
		cLocalPosition = local_origin;
		Vec3f originalscale = ECS->GetComponent<Transform>(cBeamSource).GetScale();
		ECS->GetComponent<Transform>(cBeamSource).AttachParent(attacker);
		ECS->GetComponent<Transform>(cBeamSource).SetLocalPosition(local_origin);
		ECS->GetComponent<Transform>(cBeamSource).SetScale(originalscale);
		ECS->GetComponent<EntityInfo>(cBeamSource).SetFlag(EntityFlag::active, false);
		ECS->GetComponent<Transform>(cBeamBlast).AttachParent(attacker);
		ECS->GetComponent<EntityInfo>(cBeamBlast).SetFlag(EntityFlag::active, false);
		//ECS->GetComponent<Physics>(cBeamBlast).SetMotionType(JPH::EMotionType::Dynamic);
		//ECS->GetComponent<Physics>(cBeamBlast).SetIsTrigger(false);


		if (ECS->HasComponent<GameLogic>(beamBlast)) {
			auto scrpt_ptr = ECS->GetComponent<GameLogic>(beamBlast).GetScript<Beam>();
			Beam* beam_ptr = dynamic_cast<Beam*>(scrpt_ptr.get());
			if (beam_ptr != nullptr) {
				beam_ptr->SetFinalDamage(base_attack * sDamage_Multplr);
				beam_ptr->SetFirerID(attacker);
				beam_ptr->SetIgnoreTag(ignoreTagtarget);
			}
		}
	}

	void ChargeBeamSkill::StartCharge()
	{

	}

	void ChargeBeamSkill::UpdateCharging(float dt)
	{
		if (!cCharge.isCharged && !IsBlasting && !IsExtending && ECS->DoesEntityExists(cBeamSource))
		{
			ECS->GetComponent<EntityInfo>(cBeamSource).SetFlag(EntityFlag::active, true);
			cCharge.update_Charging(dt);

			Vec3f sourceScale = ECS->GetComponent<Transform>(cBeamSource).GetScale();
			sourceScale += Vec3f(cCharge_size_multiplr) * dt;
			ECS->GetComponent<Transform>(cBeamSource).SetScale(sourceScale);

			if (cCharge.isCharged) {
				if (ECS->DoesEntityExists(cBeamBlast) && ECS->DoesEntityExists(cBeamSource)) {
					ECS->GetComponent<EntityInfo>(cBeamBlast).SetFlag(EntityFlag::active, true);
					ECS->GetComponent<Transform>(cBeamBlast).SetScale(Vec3f(sourceScale.x, sourceScale.y, 0.1f));
				}
				IsExtending = true;
			}
		}

	}

	// returns true if started blasting
	bool ChargeBeamSkill::UpdateBlasting(Vec3f const& dir_length, float dt, bool useBabySFX)
	{
		bool startedblasting = false;
		if (IsExtending && !IsBlasting) {
			if (ECS->DoesEntityExists(cBeamBlast)) {
				Vec3f blastScale = ECS->GetComponent<Transform>(cBeamBlast).GetScale();
				float sqrBlastLength = blastScale.z * blastScale.z;
				Vec3f dir_norm = dir_length.Normalised();
				Vec3f dir_extended = dir_length + dir_norm * 2.f;
				if (sqrBlastLength < dir_extended.SqMag())
				{
					blastScale.z += cSpeed * dt;
					Vec3f sourcePos = ECS->GetComponent<Transform>(cBeamSource).GetPosition();
					ECS->GetComponent<Transform>(cBeamBlast).SetScale(blastScale);
					ECS->GetComponent<Transform>(cBeamBlast).SetPosition(sourcePos + dir_norm * blastScale.z * 0.5f);
					Quatf dir_rot = Vec3f::Forward().get_rotation_between(dir_extended);
					ECS->GetComponent<Transform>(cBeamBlast).SetRotation(dir_rot);
				}
				else {
					IsExtending = false;
					IsBlasting = true;
					auto scrpt_ptr = ECS->GetComponent<GameLogic>(cBeamBlast).GetScript<Beam>();
					Beam* beam_ptr = dynamic_cast<Beam*>(scrpt_ptr.get());
					if (beam_ptr != nullptr) {
						beam_ptr->Enable(true);
					}
					AudioSource& as = ECS->GetComponent<AudioSource>(GLS->GetPlayerID());
					if (useBabySFX) {
						as.Play(laserFireSFX);
					}
					as.Play(laserFire2SFX);
					startedblasting = true;
				}
			}
		}
		if (IsBlasting && !IsExtending) {
			if (ECS->DoesEntityExists(cBeamBlast) && ECS->DoesEntityExists(cBeamSource))
			{
				Vec3f dir_norm = dir_length.Normalised();
				Vec3f dir_extended = dir_length + dir_norm * 2.f;
				Vec3f sourcePos = ECS->GetComponent<Transform>(cBeamSource).GetPosition();
				Vec3f blastScale = ECS->GetComponent<Transform>(cBeamBlast).GetScale();
				blastScale.z = dir_extended.Mag();
				// set laser size, position and rotation
				ECS->GetComponent<Transform>(cBeamBlast).SetPosition(sourcePos + dir_extended * 0.5f);
				ECS->GetComponent<Transform>(cBeamBlast).SetScale(blastScale);
				ECS->GetComponent<Physics>(cBeamBlast).SetShapeSize(blastScale);
				Quatf dir_rot = Vec3f::Forward().get_rotation_between(dir_extended);
				ECS->GetComponent<Transform>(cBeamBlast).SetRotation(dir_rot);
			}
			cBlast_timer += dt;
			if (cBlast_timer >= cBlast_duration) {
				cBlast_timer = 0.f;
				IsBlasting = false;
				CanCloseBeam = true;
				auto scrpt_ptr = ECS->GetComponent<GameLogic>(cBeamBlast).GetScript<Beam>();
				Beam* beam_ptr = dynamic_cast<Beam*>(scrpt_ptr.get());
				if (beam_ptr != nullptr) {
					beam_ptr->Enable(false);
				}
				AudioSource& as = ECS->GetComponent<AudioSource>(GLS->GetPlayerID());
				as.Stop(laserFire2SFX);
				sCooldown.start = true;
			}
		}

		return startedblasting;
	}

	void ChargeBeamSkill::UpdateEndCharge(Vec3f const& dir_length, float dt)
	{
		if (CanCloseBeam && ECS->DoesEntityExists(cBeamBlast) && ECS->DoesEntityExists(cBeamSource))
		{
			Vec3f sourceScale = ECS->GetComponent<Transform>(cBeamSource).GetScale();
			if (sourceScale.z > 0.0f) {
				sourceScale -= 2.0f * dt;
				ECS->GetComponent<Transform>(cBeamSource).SetScale(sourceScale);

				Vec3f dir_norm = dir_length.Normalised();
				Vec3f dir_extended = dir_length + dir_norm * 2.f;
				Vec3f sourcePos = ECS->GetComponent<Transform>(cBeamSource).GetPosition();
				ECS->GetComponent<Transform>(cBeamBlast).SetPosition(sourcePos + dir_extended * 0.5f);
				Quatf dir_rot = Vec3f::Forward().get_rotation_between(dir_extended);
				ECS->GetComponent<Transform>(cBeamBlast).SetRotation(dir_rot);
				Vec3f blastScale = Vec3f(sourceScale.z, sourceScale.z, dir_extended.Mag());
				ECS->GetComponent<Transform>(cBeamBlast).SetScale(blastScale);
			}
			else {
				endCharge = true;
				CanCloseBeam = false;
			}
		}
		else if (endCharge) {
			CancelCharge();
			endCharge = false;
			IsChargeStart = false;
		}
	}

	void  ChargeBeamSkill::CancelCharge() {
		if (ECS->DoesEntityExists(cBeamBlast)) {
			ECS->GetComponent<EntityInfo>(cBeamBlast).SetFlag(EntityFlag::active, false);
			ECS->GetComponent<Transform>(cBeamBlast).SetScale(Vec3f(cRadius, cRadius, 0.f));
		}
		if (ECS->DoesEntityExists(cBeamSource)) {
			ECS->GetComponent<EntityInfo>(cBeamSource).SetFlag(EntityFlag::active, false);
			ECS->GetComponent<Transform>(cBeamSource).SetScale(Vec3f(cRadius));
		}

		AudioSource& as = ECS->GetComponent<AudioSource>(GLS->GetPlayerID());
		as.Stop(laserFire2SFX);

		cCharge.reset_Charge();
		IsBlasting = false;
		IsExtending = false;
		CanCloseBeam = false;
		endCharge = false;
		IsChargeStart = false;
		cBlast_timer = 0.f;
	}

	void Beam::OnPersistedCollision(BEID collidingID)
	{
		if (!IgnoreTag.empty() && ECS->GetComponent<EntityInfo>(collidingID).HasTag(IgnoreTag)) return;
		if (enabled && collidingID != firer) {
			DamageTarget(collidingID);
		}
	}

	void Beam::OnCollision(BEID collidingID)
	{
		if (ECS->GetComponent<Physics>(collidingID).GetIsVirtualCharacter()) {
			if (IgnoreTag.size() && ECS->GetComponent<EntityInfo>(collidingID).HasTag(IgnoreTag)) return;
			if (enabled && collidingID != firer) {
				DamageTarget(collidingID, true);
			}
		}
	}


	inline void Beam::DamageTarget(BEID target, bool isvirtual)
	{
		if (ECS->HasComponent<GameLogic>(target)) {
			auto scrpt_ptr = ECS->GetComponent<GameLogic>(target).GetScript<CharacterScript>();
			CharacterScript* char_ptr = dynamic_cast<CharacterScript*>(scrpt_ptr.get());
			if (char_ptr != nullptr) {
				float frameDamage = pDamage_Final * (float)Engine->GetFixedDeltaTime();
				frameDamage = ECS->GetComponent<EntityInfo>(target).HasTag("Boss") ? frameDamage * 0.75f : frameDamage;
				char_ptr->DamageHealth(frameDamage);
				if(isvirtual) char_ptr->KnockBackCharacter(ECS->GetComponent<Transform>(target).GetPosition() - ECS->GetComponent<Transform>(firer).GetPosition(), KnockBack);
				if (ECS->GetComponent<GameLogic>(target).HasScript<EnemyBase>())
				{
					AudioSource& as = ECS->GetComponent<AudioSource>(target);
					if (!as.IsAudioPlaying(AssetID(8629549385509738384)))//WaterSizzle
					{
						//std::cout << target << " sizzle\n";
						as.Play(AssetID(8629549385509738384));
					}
				}
			}

		}
	}
}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE::Serialisation;
	using namespace BE;
	using namespace TH;
	// ENUMS REGISTRAIONS
	rttr::registration::enumeration<StatusType>(RegistrationNameCreator<StatusType>())
	(
		value("NoStatus", StatusType::NoStatus),
		value("Stun", StatusType::Stun),
		value("Burn", StatusType::Burn)
	);
	rttr::registration::enumeration<ProjectileSkill::ProjectileType>(RegistrationNameCreator<ProjectileSkill::ProjectileType>())
	(
		value("Straight", ProjectileSkill::ProjectileType::Straight),
		value("Weighted", ProjectileSkill::ProjectileType::Weighted)
	);
	rttr::registration::enumeration<AttackSkill::AOEType>(RegistrationNameCreator<AttackSkill::AOEType>())
	(
		value("Single", AttackSkill::AOEType::Single),
		value("AABox", AttackSkill::AOEType::AABox),
		value("Sphere", AttackSkill::AOEType::Sphere)
	);

	// DAMAGING SCRIPTS REGISTRATION
	registration::class_<Projectile>(RegistrationNameCreator<Projectile>())
		.constructor();

	registration::class_<Beam>(RegistrationNameCreator<Beam>())
		.constructor();

	// SKILLS STRUCTS REGISTRATION
	registration::class_<AttackSkill>(BE::RegistrationNameCreator<AttackSkill>())
		.constructor()
		.property("CoolDown Duration", &AttackSkill::GetCoolDownDuration, &AttackSkill::SetCoolDownDuration)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Damage Multiplier", &AttackSkill::sDamage_Multplr)
		.property("Cast Range", &AttackSkill::sCast_Range)
		.property("KnockBack", &AttackSkill::sKnockBack)
		.property("AOE Size", &AttackSkill::aAOE_size)
		.property("AOE Type", &AttackSkill::aAOE_type);

	registration::class_<ProjectileSkill>(BE::RegistrationNameCreator<ProjectileSkill>())
		.constructor()
		.property("CoolDown Duration", &ProjectileSkill::GetCoolDownDuration, &ProjectileSkill::SetCoolDownDuration)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
			)
		.property("Damage Multiplier", &ProjectileSkill::sDamage_Multplr)
		.property("Status Effect", &ProjectileSkill::pStatus_effect)
		.property("Status Duration", &ProjectileSkill::pStatus_duration)
		.property("Cast Range", &ProjectileSkill::sCast_Range)
		.property("KnockBack", &ProjectileSkill::sKnockBack)
		.property("Projectile Name", &ProjectileSkill::pName)
		.property("Projectile Type", &ProjectileSkill::pProjectile_type)
		.property("Projectile Radius", &ProjectileSkill::pRadius)
		.property("Projectile Speed", &ProjectileSkill::pSpeed);

	registration::class_<ChargedProjectile>(BE::RegistrationNameCreator<ChargedProjectile>())
		.constructor()
		.property("Projectile Base Skill", &ChargedProjectile::GetBase, &ChargedProjectile::SetBase)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Charge Duration", &ChargedProjectile::cCharge_duration)
		.property("Charge Size Multiplier", &ChargedProjectile::cCharge_size_multiplr);

	registration::class_<RayCastSkill>(BE::RegistrationNameCreator<RayCastSkill>())
		.constructor()
		.property("CoolDown Duration", &RayCastSkill::GetCoolDownDuration, &RayCastSkill::SetCoolDownDuration)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
			)
		.property("Damage Multiplier", &RayCastSkill::sDamage_Multplr)
		.property("Cast Range", &RayCastSkill::sCast_Range)
		.property("KnockBack", &RayCastSkill::sKnockBack)
		.property("Raycast Speed", &RayCastSkill::rSpeed)
		.property("Raycast Length", &RayCastSkill::rRayLength)
		.property("Raycast Type", &RayCastSkill::rRayType);
}
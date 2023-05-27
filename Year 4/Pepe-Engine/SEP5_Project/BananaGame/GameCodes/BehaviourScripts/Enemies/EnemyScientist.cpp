
#include "EnemyScientist.h"

namespace TH
{
	using namespace BE;
	void EnemyScientist::Awake()
	{
		if (!ECS->HasComponent<AudioSource>(id))
			ECS->AddComponent<AudioSource>(id);
		EnemySpecialized::Awake();
		auto children = ECS->GetComponent<Transform>(id).GetChildren();
		for (auto child : children) {
			if (ECS->HasComponent<Renderer>(child)) {
				eModelID = child;
				break;
			}
		}
		if (eModelID == NULL_ENTITY)
			eModelID = id;
		charac_base->SetModelID(eModelID);
		eEnemy_Type = EnemyType::Ranged;
	}
	void EnemyScientist::Init()
	{
		EnemySpecialized::Init();
		//float ranged_Damage = 1.f;
		//float ranged_CD = 2.5f;
		//float projectile_Size = 0.2f;
		//float ranged_Speed = 20.0f;
		//eRange_attack = ChargedProjectile(ranged_CD, ranged_Damage, projectile_Size, ranged_Speed, ProjectileSkill::ProjectileType::Straight, "pRobot_Bullet", 10.f);
		//eRange_attack.pProjectile_type = ProjectileSkill::ProjectileType::Straight;
		//eRange_attack.pName = "pRobot_Bullet";
		//eRange_attack.cCharge_duration = 1.f;
		//eRange_attack.cCharge_size_multiplr = 2.f;
		eRange_attack.pIgnoreTag = "Enemy";
		eRange_attack.pDestroyOnHitAny = true;
		// init stats
		//charac_base->SetBaseAttack(3.f);
		//charac_base->SetTotalHealth(100.f);
		//charac_base->SetCurrentHealth(100.f);
		//charac_base->SetAggroRange(20.f);
		//charac_base->SetDestroyDuration(0.1f);
		charac_base->SetResetRange(3000.f);
		charac_base->SetDestroyDuration(3.f);
		lastHPknown = charac_base->GetCurrentHealth();


		InitAnimationInfo(eModelID);
		SetAnimState(RangedAnim::Walk);
	}

	void EnemyScientist::Update(double dt)
	{
		UpdateTimers(static_cast<float>(dt));
		//if (lastHPknown != charac_base->GetCurrentHealth())
		//{
		//	AudioSource& as = ECS->GetComponent<AudioSource>(id);
		//	if(!as.IsAudioPlaying(AssetID(10210675003471560135)))
		//		as.Play(AssetID(10210675003471560135));					//RobotDamaged
		//	lastHPknown = charac_base->GetCurrentHealth();
		//}
	}

	inline void EnemyScientist::UpdateTimers(float dt)
	{
		Vec3f dir;
		if (charac_base->HasTarget()) dir = ECS->GetComponent<Transform>(charac_base->GetTarget()).GetPosition() - ECS->GetComponent<Transform>(id).GetPosition();
		eRange_attack.UpdateCharging(dir, dt, charac_base->IsStunned());
		eRange_attack.UpdateCoolDown(dt);

	}

	void EnemyScientist::InitAnimationInfo(EntityID modelID)
	{
		if (ECS->HasComponent<AnimationComponent>(modelID) && ECS->HasComponent<Renderer>(modelID))
		{
			//Check if model is loaded
			if (AssetMgr->IsAssetDataLoaded(ECS->GetComponent<Renderer>(modelID).getModelID()))
			{
				//now go through all animations
				auto& vector = AssetMgr->GetAssetData<BeModel>(ECS->GetComponent<Renderer>(modelID).getModelID()).GetAnimationDetails();
				for (size_t i{}; i < static_cast<size_t>(RangedAnim::Total_Animations); ++i)
				{
					//std::cout << vector[i].GetCurrentAnimationName() << "\n";
					if (i >= pAnim_Indexes.size()) continue;
					if (pAnim_Indexes[i].index < 0) {
						pAnim_Indexes[i].index = static_cast<int>(i);
						pAnim_Indexes[i].duration = vector[i].GetDuration() / vector[i].GetTicksPerSecond();
					}
				}
			}
		}
	}

	void EnemyScientist::AutoResetAnimation(float dt)
	{
		if (!ECS->HasComponent<AnimationComponent>(eModelID)) return;
		HasSetAnimation_frame = false;
		if (!pIsPlayingOneShot) return;

		if (ECS->GetComponent<AnimationComponent>(eModelID).getCompletedOnce()) {
			pIsPlayingOneShot = false;
			SetAnimState(RangedAnim::Walk);
		}
	}

	bool EnemyScientist::SetAnimState(RangedAnim in, bool forced)
	{
		if (!ECS->HasComponent<AnimationComponent>(eModelID)) return false;
		size_t index = static_cast<size_t>(in);
		// Check if animation was found and loaded.
		if (pAnim_Indexes[index].index < 0) return false;
		if (in == pCurrent_Anim) return false;

		if (!forced) {
			if (pIsPlayingOneShot || HasSetAnimation_frame) return false;
			else pPrev_Anim = pCurrent_Anim;
		}
		else {
			if (pIsPlayingOneShot) pIsPlayingOneShot = false;
		}

		pCurrent_Anim = in;
		ECS->GetComponent<AnimationComponent>(eModelID).SetAnimation(pAnim_Indexes[index].index);
		HasSetAnimation_frame = true;

		ECS->GetComponent<AnimationComponent>(eModelID).setTickFactor(1.f);
		ECS->GetComponent<AnimationComponent>(eModelID).setFreezeAtLastFrame(false);

		switch (in) {
		case RangedAnim::Damage: {

			pIsPlayingOneShot = true;
			break;
		}
		case RangedAnim::Attack: {
			ECS->GetComponent<AnimationComponent>(eModelID).setTickFactor(0.75f);
			pIsPlayingOneShot = true;
			break;
		}
		case RangedAnim::Death: {
			ECS->GetComponent<AnimationComponent>(eModelID).setFreezeAtLastFrame(true);

			pIsPlayingOneShot = true;
			break;
		}
		case RangedAnim::Walk: {
			//ECS->GetComponent<AnimationComponent>(eModelID).setTickFactor(1.f);
			break;
		}
		default: {
			break;
		}
		}

		return true;
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<EnemyScientist>(RegistrationNameCreator<EnemyScientist>())
		.constructor()
		.property("Charged Ranged Attack", &EnemyScientist::eRange_attack);
}

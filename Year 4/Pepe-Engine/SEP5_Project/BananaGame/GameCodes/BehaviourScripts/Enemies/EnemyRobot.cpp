
#include "EnemyRobot.h"

namespace TH
{
	using namespace BE;

	void EnemyRobot::Awake()
	{
		EnemySpecialized::Awake();
		auto children = ECS->GetComponent<Transform>(id).GetChildren();
		for (auto child : children) {
			if (ECS->HasComponent<Renderer>(child)) {
				eModelID = child;
				break;
			}
		}
		if(eModelID == NULL_ENTITY)
			eModelID = id;
		charac_base->SetModelID(eModelID);
		eEnemy_Type = EnemyType::Melee;
	}
	void EnemyRobot::Init()
	{
		EnemySpecialized::Init();
		//float melee_Damage = 2.f;
		//float melee_CD = 2.0f;
		//eMelee_attack = AttackSkill(melee_CD, melee_Damage, BE::Vec3f(2.5f, 3.f, 2.5f), AttackSkill::AOEType::AABox, 2.5f);

		// init stats
		/*charac_base->SetBaseAttack(3.f);
		charac_base->SetBaseSpeed(4.f);
		charac_base->SetTotalHealth(100.f);
		charac_base->SetCurrentHealth(100.f);
		charac_base->SetAggroRange(20.f);
		charac_base->SetDestroyDuration(3.f);*/
		charac_base->SetResetRange(3000.f);
		InitAnimationInfo(eModelID);
		SetAnimState(RobotAnim::Walk);
		lastHPknown = charac_base->GetCurrentHealth();
	}

	void EnemyRobot::Update(double dt)
	{
		UpdateTimers(static_cast<float>(dt));
		if (CastedAttack && !pIsPlayingOneShot) {
			MeleeAttackDamage();
			CastedAttack = false;
		}
		//if (lastHPknown != charac_base->GetCurrentHealth())
		//{
		//	AudioSource& as = ECS->GetComponent<AudioSource>(id);
		//	if (!as.IsAudioPlaying(AssetID(10210675003471560135)))
		//		as.Play(AssetID(10210675003471560135));					//RobotDamaged
		//	lastHPknown = charac_base->GetCurrentHealth();
		//}
	}

	inline void EnemyRobot::UpdateTimers(float dt)
	{
		eMelee_attack.UpdateCoolDown(dt);
	}

	void EnemyRobot::InitAnimationInfo(EntityID modelID)
	{
		if (ECS->HasComponent<AnimationComponent>(modelID) && ECS->HasComponent<Renderer>(modelID))
		{
			//Check if model is loaded
			if (AssetMgr->IsAssetDataLoaded(ECS->GetComponent<Renderer>(modelID).getModelID()))
			{
				//now go through all animations
				auto& vector = AssetMgr->GetAssetData<BeModel>(ECS->GetComponent<Renderer>(modelID).getModelID()).GetAnimationDetails();
				for (int i{}; i < vector.size(); ++i)
				{
					//std::cout << vector[i].GetCurrentAnimationName() << "\n";
					if (i >= pAnim_Indexes.size()) continue;
					if (pAnim_Indexes[i].index < 0) {
						pAnim_Indexes[i].index = i;
						pAnim_Indexes[i].duration = vector[i].GetDuration() / vector[i].GetTicksPerSecond();
					}
				}
			}
		}
	}
	void EnemyRobot::AutoResetAnimation(float dt)
	{
		HasSetAnimation_frame = false;
		if (!pIsPlayingOneShot) return;

		if (ECS->GetComponent<AnimationComponent>(eModelID).getCompletedOnce()) {
			pIsPlayingOneShot = false;
			SetAnimState(RobotAnim::Walk);
		}
	}

	bool EnemyRobot::SetAnimState(RobotAnim in, bool forced)
	{
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
		case RobotAnim::Damage: {
			pIsPlayingOneShot = true;
			break;
		}
		case RobotAnim::Attack: {
			ECS->GetComponent<AnimationComponent>(eModelID).setTickFactor(0.75f);
			pIsPlayingOneShot = true;
			break;
		}
		case RobotAnim::Death: {
			ECS->GetComponent<AnimationComponent>(eModelID).setFreezeAtLastFrame(true);
			
			pIsPlayingOneShot = true;
			break;
		}
		case RobotAnim::Slip: {
			pIsPlayingOneShot = true;
			break;
		}
		case RobotAnim::Walk: {
			ECS->GetComponent<AnimationComponent>(eModelID).setTickFactor(0.75f);
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
	registration::class_<EnemyRobot>(RegistrationNameCreator<EnemyRobot>())
		.constructor()
		.property("Melee Attack", &EnemyRobot::eMelee_attack);
}
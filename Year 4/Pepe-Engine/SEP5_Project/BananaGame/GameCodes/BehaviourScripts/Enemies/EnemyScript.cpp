
#include "EnemyScript.h"

namespace TH
{
	using namespace BE;

	EnemyBase::EnemyFSM EnemyBase::fsm;
	EnemyBase::EnemyStatics EnemyBase::statics;

	template <typename T>
	void AttackState<T>::Update(T & script, double dt)
	{ 
		if (!script.SkipOnce) {
			if (script.charac_base->NoTarget()) {
				return;
			}
			script.SkipOnce = true;
		}

		Vec3f dir_to_rot = script.charac_base->GetNormXZDirectionToEntity(script.charac_base->GetTarget());
		ECS->GetComponent<Transform>(script.id).SetRotation(Vec3f::Forward().get_rotation_between(dir_to_rot));

		if (!script.enemy_specialize->GetBaseSkill()->IsOnCoolDown() && 
			!script.enemy_specialize->IsAttackCharging()) {
			bool playedanim = false;
			if (!script.enemy_specialize->pIsPlayingOneShot)
				playedanim = script.enemy_specialize->BaseSetAnimation(EnemyBaseAnims::Attack);

			if(playedanim) script.enemy_specialize->CastAttack();
		}
	}
	template <typename T>
	void AttackState<T>::Leave(T& script)
	{
		script.SkipOnce = false;
		if (script.enemy_specialize->IsAttackCharging()) {
			script.enemy_specialize->CancelCharge();
		}
	}

	template <typename T>
	void IdleState<T>::Enter(T& script)
	{
		//LinearDamping = ECS->GetComponent<Physics>(script.id).GetLinearDamping();
		ECS->GetComponent<Physics>(script.id).SetLinearDamping(0.f);
	}

	template <typename T>
	void IdleState<T>::Update(T& script, double dt)
	{ 
		// check player's distance if within aggro range, then do LOS
		float sqr_dist = script.SqrDistFromEntity(GLS->GetPlayerID());
		float aggro_range = script.charac_base->GetAggroRange();
		if (sqr_dist < aggro_range* aggro_range)
		{
			if (script.charac_base->GetLOStoEntity(GLS->GetPlayerID())) {
				script.charac_base->SetTarget(GLS->GetPlayerID());
			}
		}
	}

	template <typename T>
	void IdleState<T>::Leave(T& script)
	{
		ECS->GetComponent<Physics>(script.id).SetLinearDamping(script.LinearDamping);
	}

	template <typename T>
	void AggroPathingState<T>::Update(T& script, double dt)
	{ 
		if (!script.enemy_specialize->pIsPlayingOneShot)
			script.enemy_specialize->BaseSetAnimation(EnemyBaseAnims::Walk);

		if (!script.SkipOnce) {
			if (script.charac_base->NoTarget()) {
				return;
			}
			script.SkipOnce = true;
		}

		if (script.IsMelee && script.statics.playerSurround) {
			Vec3f toPathto;
			script.surroundPosition = script.statics.playerSurround->UpdateCurrentSpot(script.surroundPosition, ECS->GetComponent<Transform>(script.id).GetPosition(), script.playerOldPos);
			if (script.surroundPosition != SurroundPos(0,0)) {
				toPathto = script.statics.playerSurround->GetWorldPositionAt(script.surroundPosition);
				//pathfinding here
				script.charac_pather->PathTo(script.charac_base->GetBaseSpeed(), toPathto, script.charac_base->GetTarget());
			}
			
		}
		else {
			script.charac_pather->PathTo(script.charac_base->GetBaseSpeed(), ECS->GetComponent<Transform>(script.charac_base->GetTarget()).GetPosition(), script.charac_base->GetTarget());
		}

		if (script.charac_pather->NoPath()) {
			Vec3f dir_to_rot = script.charac_base->GetNormXZDirectionToEntity(script.charac_base->GetTarget());
			ECS->GetComponent<Transform>(script.id).SetRotation(Vec3f::Forward().get_rotation_between(dir_to_rot));
		}
		//auto oldsurround = script.surroundPosition;
		//Vec3f velocity = in.GetNormXZDirectionToEntity(script.charac_base->GetTarget()) * in.charac_base->GetBaseSpeed();
		//ECS->GetComponent<Physics>(in.id).SetLinearVelocity(velocity);
	}

	template <typename T>
	void AggroPathingState<T>::Leave(T& script)
	{
		script.SkipOnce = false;
		script.charac_pather->ClearPath();
		//ECS->GetComponent<Physics>(script.id).SetLinearVelocity({0.f, 0.f, 0.f});
	}

	template <typename T>
	void ResetPathingState<T>::Enter(T& script)
	{ 
		script.charac_base->SetInvulnerable(true);
	}

	template <typename T>
	void ResetPathingState<T>::Update(T& script, double dt)
	{ 
		if (!script.enemy_specialize->pIsPlayingOneShot)
			script.enemy_specialize->BaseSetAnimation(EnemyBaseAnims::Walk);
		//add pathfinding here
		script.charac_pather->PathTo(script.charac_base->GetBaseSpeed(), script.charac_base->GetSpawnPos());
		//Vec3f velocity = script.GetNormXZDirectionToPosition(script.charac_base->GetSpawnPos()) * script.charac_base->GetBaseSpeed();
		//ECS->GetComponent<Physics>(script.id).SetLinearVelocity(velocity);
	}

	template <typename T>
	void ResetPathingState<T>::Leave(T& script)
	{ 
		script.charac_base->SetInvulnerable(false);
		script.charac_base->SetTarget(NULL_ENTITY);
		script.charac_pather->ClearPath();

		//ECS->GetComponent<Physics>(script.id).SetLinearVelocity({ 0.f, 0.f, 0.f });
	}

	template <typename T>
	void DeadState<T>::Enter(T& script)
	{ 
		//std::cout << ECS->GetComponent<EntityInfo>(script.id).GetName() << " is Dead!\n";
		//ECS->GetComponent<Physics>(script.id).SetInvalidateCollide(true);
		ECS->GetComponent<Physics>(script.id).SetLinearVelocity({ 0.f, 0.f, 0.f });
		ECS->GetComponent<Physics>(script.id).SetIsTrigger(true);
		ECS->GetComponent<Physics>(script.id).SetGravityFactor(0.f);
		script.enemy_specialize->BaseSetAnimation(EnemyBaseAnims::Death, true);

		//AudioSource& aSource = ECS->GetComponent<AudioSource>(script.id);
		//aSource.Play(script.SFX_Death);
	}

	template <typename T>
	void StunState<T>::Enter(T& script)
	{
		script.enemy_specialize->BaseSetAnimation(EnemyBaseAnims::Slip);
		ECS->GetComponent<Physics>(script.id).SetLinearVelocity({ 0.f, 0.f, 0.f });
		ECS->GetComponent<Physics>(script.id).SetAngularVelocity({ 0.f, 0.f, 0.f });
	}

	template <typename T>
	void StunState<T>::Leave(T& script)
	{
		script.enemy_specialize->BaseSetAnimation(EnemyBaseAnims::Walk);
	}

	void EnemyBase::Awake()
	{
	}

	void EnemyBase::Init()
	{
		//The priority of transitions condition checking is in the order of when it is added 	
		fsm.Init(current_state, previous_state);

		// Get pointer to character base
		auto script_ptr = BE::ECS->GetComponent<BE::GameLogic>(id).GetScript<CharacterScript>();
		if (script_ptr.get() == nullptr) {
			ECS->GetComponent<GameLogic>(id).AddScript<CharacterScript>();
			script_ptr = ECS->GetComponent<GameLogic>(id).GetScript<CharacterScript>();
		}
		charac_base = dynamic_cast<CharacterScript*>(script_ptr.get());

		// Get pointer to character pather
		script_ptr = BE::ECS->GetComponent<BE::GameLogic>(id).GetScript<CharacterPather>();
		if (script_ptr.get() == nullptr) {
			ECS->GetComponent<GameLogic>(id).AddScript<CharacterPather>();
			script_ptr = ECS->GetComponent<GameLogic>(id).GetScript<CharacterPather>();
		}
		charac_pather = dynamic_cast<CharacterPather*>(script_ptr.get());

		script_ptr = ECS->GetComponent<GameLogic>(GLS->GetPlayerID()).GetScript<PlayerSurround>();
		PlayerSurround* playerSur = dynamic_cast<PlayerSurround*>(script_ptr.get());
		if (playerSur) statics.playerSurround = playerSur;
		

		ECS->GetComponent<Physics>(id).SetGravityFactor(1.f);
			
		if (enemy_specialize->GetEnemyType() == EnemyType::Melee) {
			IsMelee = true;
		}

		enemy_specialize->BaseSetAnimation(EnemyBaseAnims::Walk, true);
		//AudioSource& aSource = ECS->GetComponent<AudioSource>(id);
		//aSource.AddAudio(SFX_Death, 0.2f, 1.f, 128, false, false, 1.0, 100);
	}

	void EnemyBase::Update(double dt)
	{
		if (IsDead()) {
			if (surroundPosition != SurroundPos(0, 0)) {
				if(statics.playerSurround) statics.playerSurround->RemovePositionalCount(surroundPosition);
				surroundPosition = SurroundPos(0, 0);
			}
			fsm.Update(current_state, previous_state, dt);
			return;
		}
		if (charac_base->HasTarget()) {
			auto scrpt_ptr = ECS->GetComponent<GameLogic>(charac_base->GetTarget()).GetScript<CharacterScript>();
			CharacterScript* char_ptr = dynamic_cast<CharacterScript*>(scrpt_ptr.get());
			if (char_ptr) {
				if(char_ptr->IsDead())
					charac_base->SetTarget(NULL_ENTITY);
			}
			else charac_base->SetTarget(NULL_ENTITY);
		}
		enemy_specialize->AutoResetAnimation(static_cast<float>(dt));
		UpdateTimers(static_cast<float>(dt));

		//if (oldPosition.SqDistBetween(ECS->GetComponent<Transform>(id).GetPosition()) >= statics.tile_size * statics.tile_size)
		//{
		//	charac_pather->UpdateGridOccupied(oldPosition, ECS->GetComponent<Transform>(id).GetPosition());
		//	oldPosition = ECS->GetComponent<Transform>(id).GetPosition();
		//}

		if (charac_base->cTookDamageFrame) {
			enemy_specialize->BaseSetAnimation(EnemyBaseAnims::Damage, true);
			charac_base->cTookDamageFrame = false;
		}
		fsm.Update(current_state, previous_state, dt);

		if (!enemy_specialize->pIsPlayingOneShot)
			enemy_specialize->BaseSetAnimation(EnemyBaseAnims::Walk);
	}
	void EnemyBase::Cleanup()
	{
	}
	// calls by System when the behaviour instance is asleep, if returns true, wakes the behaviour itself
	bool EnemyBase::SelfWakeCondition()
	{
		return false;
	}
	void EnemyBase::UponSleep()
	{

	}
	void EnemyBase::UponWake()
	{
	}

	void EnemyBase::OnCollision(EntityID collidingID)
	{
	}

	void EnemyBase::OnPersistedCollision(EntityID collidingID)
	{
		if (IsDead()) return;
	}
	void EnemyBase::OnCollisionRemoved(EntityID collidingID)
	{
	}
	inline void EnemyBase::UpdateTimers(float dt)
	{
	}

	inline bool EnemyBase::TargetIsWithin_AttackRange() const
	{
		float sqr_dist = SqrDistToTargetColliderFront(charac_base->GetTarget());
		float range = enemy_specialize->GetBaseSkill()->sCast_Range;
		return sqr_dist < (range * range * 0.9f * 0.9f) && charac_base->GetLOStoEntity(charac_base->GetTarget());
	}

	inline bool EnemyBase::TargetNotWithinReset() const
	{
		float sqr_dist = ECS->GetComponent<Transform>(charac_base->GetTarget()).GetPosition().SqDistBetween(charac_base->GetSpawnPos());
		return sqr_dist > (charac_base->GetResetRange() * charac_base->GetResetRange());
	}

	inline bool EnemyBase::IsBackAtSpawn() const
	{
		return ECS->GetComponent<Physics>(id).BoundingBoxContainsPoint(charac_base->GetSpawnPos());
	}

	inline bool EnemyBase::TargetNotWithin_AttackRange() const
	{
		float sqr_dist = SqrDistToTargetColliderFront(charac_base->GetTarget());
		float range = enemy_specialize->GetBaseSkill()->sCast_Range;
		return sqr_dist > (range * range) || !charac_base->GetLOStoEntity(charac_base->GetTarget());
	}

	inline float EnemyBase::SqrDistFromEntity(EntityID in) const
	{
		return ECS->GetComponent<Transform>(in).GetPosition().SqDistBetween(ECS->GetComponent<Transform>(id).GetPosition());
	}

	inline bool EnemyBase::IsAtPoint(Vec3f const& point) const
	{
		return ECS->GetComponent<Physics>(id).BoundingBoxContainsPoint(point);
	}

	void EnemySpecialized::Awake()
	{
		//Get pointer to character base
		auto script_ptr = BE::ECS->GetComponent<BE::GameLogic>(id).GetScript<CharacterScript>();
		if (script_ptr.get() == nullptr) {
			ECS->GetComponent<GameLogic>(id).AddScript<CharacterScript>();
			script_ptr = ECS->GetComponent<GameLogic>(id).GetScript<CharacterScript>();
		}
		charac_base = dynamic_cast<CharacterScript*>(script_ptr.get());

		//Tell the enemy base what specialized enemy it is.
		script_ptr = BE::ECS->GetComponent<BE::GameLogic>(id).GetScript<EnemyBase>();
		if (script_ptr.get() == nullptr) {
			ECS->GetComponent<GameLogic>(id).AddScript<EnemyBase>();
			script_ptr = ECS->GetComponent<GameLogic>(id).GetScript<EnemyBase>();
		}
		EnemyBase* enemy_base = dynamic_cast<EnemyBase*>(script_ptr.get());
		enemy_base->SetSpecialization(this);
	}
	void EnemySpecialized::Init()
	{
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<EnemyBase>(RegistrationNameCreator<EnemyBase>())
		.constructor();
}
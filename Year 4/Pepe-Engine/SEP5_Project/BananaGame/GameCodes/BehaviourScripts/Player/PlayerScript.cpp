#include "PlayerScript.h"
#include "../Environment/Interactable.h"
#include "../Level/LevelManager.h"
#include "../../UserInterface/InGameUIScripts/TutorialUIController.h"
#include "../../DialogueCutsceneObjectivesControl/DialogueController.h"
#include "../../DialogueCutsceneObjectivesControl/CutsceneController.h"
//#define TEEHEE_IS_GOD

namespace TH
{
	using namespace BE;
	static EntityID idGameManager = NULL_ENTITY;
	static const AssetID laserChargeSFX = AssetID(3861209379069743978);
	static const glm::vec2 idleSFXRange = { 3.5f,10.0f };
	static const std::vector<BE::AssetID> idleSFXID
	{
		BE::AssetID(881495705680642051),//Teehee.wav
		BE::AssetID(4588557280301885294),//OOAA.wav
		BE::AssetID(11295828875988965758),//Giggle.wav
	};
	const std::vector<BE::AssetID> attackSFXID
	{
		BE::AssetID(4325699621427970484),//Teehee.wav
		BE::AssetID(17800790814067925573),//OOAA.wav
		BE::AssetID(15254171908327527725),//Giggle.wav
	};
	float intialDialogueDelay = 0.75f;
	inline void PlayerCharacter::HandleInput(Vec3f inMovementDirection, bool isRunning, bool inJump, bool inSwitchStance, float inDeltaTime)
	{
		auto& pCharacter = charac_body->cCharacter;
		// Cancel movement in opposite direction of normal when sliding
		JPH::CharacterVirtual::EGroundState ground_state = pCharacter->GetGroundState();
		if (ground_state == JPH::CharacterVirtual::EGroundState::Sliding)
		{
			Vec3f normal = JPHtoBE(pCharacter->GetGroundNormal());
			normal.y = 0.0f;
			float dot = normal.Dot(inMovementDirection);
			if (dot < 0.0f)
				inMovementDirection -= (dot * normal) / normal.SqMag();
		}
		AudioSource& asFootSteps = ECS->GetComponent<AudioSource>(id);
		if (ground_state == JPH::CharacterVirtual::EGroundState::OnGround && inMovementDirection.SqMag() > 0.75f)
		{
			if (!asFootSteps.IsAudioPlaying(AssetID(14559977671230902046)))	//Footstep
				asFootSteps.Play(AssetID(14559977671230902046));
		}
		else
			asFootSteps.Stop(AssetID(14559977671230902046));
		// Smooth the player input
		pSmoothMovementDirection = 0.25f * inMovementDirection + 0.75f * pSmoothMovementDirection;

		Vec3f current_vertical_velocity = Vec3f(0.f, pCharacter->GetLinearVelocity().GetY(), 0);

		Vec3f ground_velocity = JPHtoBE(pCharacter->GetGroundVelocity());

		Vec3f new_velocity;
		if (ground_state == JPH::CharacterVirtual::EGroundState::OnGround // If on ground
			&& (current_vertical_velocity.y - ground_velocity.y) < 0.1f) // And not moving away from ground
		{
			// Assume velocity of ground when on ground
			new_velocity = ground_velocity;
			// Jump
			if (inJump) {
				new_velocity += Vec3f(0.f, pJumpSpeed, 0.f);
				if(inMovementDirection == Vec3f()) SetAnimState(PlayerAnim::Jump);

				auto uiController = ECS->GetComponent<GameLogic>(idGameManager).GetScriptDerived<TutorialUIController>();
				uiController->SetTutorialComplete(TutorialUIController::TutorialIndex::JumpCheckBox);
			}	
		}
		else {
			new_velocity = current_vertical_velocity;
		}
			
		// Gravity
		new_velocity += PhySys->GetGravity() * inDeltaTime;

		// Player input
		new_velocity += pSmoothMovementDirection * charac_base->GetBaseSpeed() * (isRunning? pRunning_Multplr : 1.f );

		if (new_velocity.y > pJumpSpeed) {
			new_velocity.y = pJumpSpeed;
		}
		// Update the velocity
		pCharacter->SetLinearVelocity(JPHtoBE(new_velocity));

		// Stance switch
		if (inSwitchStance) {
			charac_body->SetShape(pCharacter->GetShape() == charac_body->cStandingShape ? pCrouchingShape : charac_body->cStandingShape);
			//std::cout << "Swapped shape\n";
		}
	}

	inline void PlayerCharacter::MeleeAttack()
	{
		Mat3f rotMatrix;
		rotMatrix.SetRotation(ECS->GetComponent<Transform>(id).GetRotation());
		Vec3f cast_origin = ECS->GetComponent<Transform>(id).GetPosition() + rotMatrix * Vec3f::Forward() * pMelee_attack.aAOE_size.z;
		std::vector<Vec3f> contact_pts;
#ifdef TEEHEE_IS_GOD
		pMelee_attack.CastSkill(id, cast_origin, 999.0f, contact_pts);
#else
		pMelee_attack.CastSkill(id, cast_origin, charac_base->GetBaseAttack(), contact_pts);
#endif // TEEHEE_IS_GOD	
		auto uiController = ECS->GetComponent<GameLogic>(idGameManager).GetScriptDerived<TutorialUIController>();
		uiController->SetTutorialComplete(TutorialUIController::TutorialIndex::MeleeAttackCheckbox);
		SetAnimState(PlayerAnim::Melee, true);
	}


	inline void PlayerCharacter::ShootBanana(bool isMoving)
	{
		--pBananas_current;
		
		//Update the text
		auto text = BE::UI->GetCanvas(bananaCountText.first)->GetUI<BE::UIFontText>(bananaCountText.second);
		text->SetText(std::to_string(pBananas_current) + "/" + std::to_string(pBananas_capacity));

		Vec3f front = charac_body->GetColliderFrontalPos();
		front.y += 0.3f;
#ifdef TEEHEE_IS_GOD
		pBanana_shoot.CastSkill_EndPoint(front, GetCrosshairContactPosition(), id, 9999.0f);
#else
		pBanana_shoot.CastSkill_EndPoint(front, GetCrosshairContactPosition(), id, charac_base->GetBaseAttack());
#endif // TEEHEE_IS_GOD
		Shot_banana = true;
		pPeel_throw.sCooldown.start = true;
		AudioSource& as = ECS->GetComponent<AudioSource>(id);
		as.Play(AssetID(2952037352432953771));//BananaShoot.wav
		auto uiController = ECS->GetComponent<GameLogic>(idGameManager).GetScriptDerived<TutorialUIController>();
		uiController->SetTutorialComplete(TutorialUIController::TutorialIndex::RangeAttackCheckBox);
		if(isMoving)SetAnimState(PlayerAnim::RunShoot);
		else SetAnimState(PlayerAnim::StandFire);
	}

	inline void PlayerCharacter::PeelThrow(bool isMoving)
	{
		Shot_banana = false;

		// Get player's direction.
		Vec3f pDir = charac_base->GetCurrentDirection();
		pDir.y = 1.f;
		Vec3f front = charac_body->GetColliderFrontalPos();
		front.y += 0.3f;
		pPeel_throw.CastSkill_Direction(front, pDir, id, charac_base->GetBaseAttack());

		AudioSource& as = ECS->GetComponent<AudioSource>(id);
		as.Play(AssetID(1661649760549127214));//BananaPeelThrow.wav

		//if(!isMoving) SetAnimState(PlayerAnim::StandReload);
	}

	void PlayerCharacter::Awake()
	{
		GLS->SetPlayerID(id);
		const auto results = ECS->FindEntitiesByName("GameManager");
		if (results.size() != 1)
		{
			AssertCore(false,"Game manager entity is not found");
		}
		idGameManager = results.front();

		const auto model = ECS->FindEntitiesByName("TeeHeeModel", 1);
		if (model.empty()) {
			LogCoreError("Error finding teehee model entity!");
		}
		else {
			pModelID = model.front();
			pModel_scale = ECS->GetComponent<Transform>(pModelID).GetScale();

			//Once the model is found, we can try to find the animations required for this model
			InitAnimationInfo(pModelID);
		}

		GetOrAddOtherRequiredScripts<CharacterScript>(charac_base, id);
		GetOrAddOtherRequiredScripts<VirtualCharacter>(charac_body, id);
		GetOrAddOtherRequiredScripts<MilkBottleUIController>(milk_bottle_ptr, id);
		GetOrAddOtherRequiredScripts<PlayerInteract>(player_interact, id);

#ifdef TEEHEE_IS_GOD
		charac_base->SetBaseSpeed(5.0f);
		charac_base->SetTotalHealth(9999.0f);
		charac_base->SetCurrentHealth(9999.0f);
#endif // TEEHEE_IS_GOD


		const auto cam = ECS->FindEntitiesByName("Camera Controller", 1);
		if (cam.empty()) {
			LogCoreError("Error finding camera controller entity!");
		}
		else {
			auto script_ptr = ECS->GetComponent<GameLogic>(cam.front()).GetScript<CameraController>();
			if (script_ptr.get() != nullptr) {
				camera_control = dynamic_cast<CameraController*>(script_ptr.get());
			}
		}

		// init stats
		//charac_base->SetBaseAttack(10.f);
		//charac_base->SetTotalHealth(200.f);
		//charac_base->SetCurrentHealth(200.f);
		charac_base->SetModelID(pModelID);
		charac_base->SetKnockBackResistance(0.f);
		charac_base->SetIsVirtual(true);
	}

	void PlayerCharacter::Init()
	{	
		pCrouchingShape = JPH::RotatedTranslatedShapeSettings(JPH::Vec3(0, 0.5f * pCharacterHeightCrouching + pCharacterRadiusCrouching, 0), JPH::Quat::sIdentity(), new JPH::CapsuleShape(0.5f * pCharacterHeightCrouching, pCharacterRadiusCrouching)).Create().Get();

		pBanana_shoot.pPrefabID = AssetMgr->GetAssetID("../Assets/Prefabs/BananaFlesh.prefab");
		pPeel_throw.pPrefabID = AssetMgr->GetAssetID("../Assets/Prefabs/BananaSkin.prefab");

		pCharge_beam = ChargeBeamSkill();
		pCharge_beam.cCharge_size_multiplr = 1.2f;
		pCharge_beam.cSpeed = 300.f;
		pCharge_beam.sDamage_Multplr = 20.f;
		pCharge_beam.cCharge.duration = 1.0f;
		pCharge_beam.cBlast_duration = 1.5f;
		pCharge_beam.SetCoolDownDuration(1.0f);


		auto pPrefabID = AssetMgr->GetAssetID("../Assets/Prefabs/LaserBeam.prefab");
		auto find_entity = ECS->FindEntitiesByName("LaserBeam", 1);
		EntityID laserBlastID;
		if (!find_entity.empty()) laserBlastID = find_entity.front();
		else laserBlastID = ECS->SpawnPrefab(pPrefabID);

		pPrefabID = AssetMgr->GetAssetID("../Assets/Prefabs/LaserSource.prefab");
		find_entity = ECS->FindEntitiesByName("LaserSource", 1);
		EntityID laserSourceID;
		if (!find_entity.empty()) laserSourceID = find_entity.front();
		else laserSourceID = ECS->SpawnPrefab(pPrefabID);

		Vec3f baby_attackpos;
		find_entity = ECS->FindEntitiesByName("Baby Monkey");
		if (find_entity.size()) {
			auto script_ptr = ECS->GetComponent<GameLogic>(find_entity.front()).GetScript<BabyMonkey>();
			BabyMonkey* bbyPtr = dynamic_cast<BabyMonkey*>(script_ptr.get());
			if (bbyPtr) {
				baby_attackpos = bbyPtr->GetAttackPos();
				baby_attackpos.z += 1.f;
			}
		}
		pCharge_beam.InitChargeBeam(id, baby_attackpos, laserBlastID, laserSourceID, charac_base->GetBaseAttack());

	
		//pCharge_beam.cCharge_size_multiplr = 1.5f;ee

		ECS->GetComponent<Renderer>(pModelID).setFlickerColor({ 0.1f, 0.f, 0.f, 0.f });
		SetAnimState(PlayerAnim::Idle);
		idleSFXTimer = Rand::Random(idleSFXRange.x, idleSFXRange.y);


		meleeButton = BE::UI->FindUIElementByName("FistAbility", 1).front();
		laserButton = BE::UI->FindUIElementByName("LaserAbility", 1).front();
		bananaButton = BE::UI->FindUIElementByName("BananaAbility", 1).front();
		shieldButton = BE::UI->FindUIElementByName("ShieldAbility", 1).front();
		bananaCountText = BE::UI->FindUIElementByName("AmmoCount", 1).front();
		bananaCountBG = BE::UI->FindUIElementByName("AmmoCountBG", 1).front();
		abilityIcon = BE::UI->FindUIElementByName("AbilityIcon", 1).front();
		crosshairTexture = BE::UI->FindUIElementByName("Crosshair", 1).front();
		initialDialogueCompleted = false;
		initialDialogueStarted = false;
		//DialogueController::inst->StartDialogue(0);
	}

	inline void PlayerCharacter::UpdateTimers(float dt)
	{
		pMelee_attack.UpdateCoolDown(dt);
		//pLaser_attack.UpdateCoolDown(dt);
		pCharge_beam.UpdateCoolDown(dt);
		pBanana_shoot.UpdateCoolDown(dt);
		pPeel_throw.UpdateCoolDown(dt);
		pModeSwitch.update(dt);
		pFeedBabyDuration.update(dt);
	}

	inline void PlayerCharacter::MeleeSkill_Input(float dt)
	{
		if (!pMelee_attack.IsOnCoolDown())
		{
			MeleeAttack();
			idleSFXTimer = idleSFXRange.y;
			static unsigned char counter = 0;
			AudioSource& as = ECS->GetComponent<AudioSource>(id);
			as.Play(attackSFXID[counter % attackSFXID.size()]);//TeeHeeAttack.wav
			++counter;
		}
	}

	inline void PlayerCharacter::BeamSkill_Input(float dt)
	{
		if (pCharge_beam.IsChargeStart) {
		
			pCharge_beam.UpdateCharging(dt);
		}
		if (!pCharge_beam.IsOnCoolDown() && pCanAttack && Input->IsMouseTriggered(Mouse::MOUSE_LEFT) && !pCharge_beam.IsInAnimation()) {
			pCharge_beam.UpdateCharging(dt);
			AudioSource& as = ECS->GetComponent<AudioSource>(id);
			if (!as.IsAudioPlaying(laserChargeSFX))
				as.Play(laserChargeSFX);//TeeHeeAttack.wav
			pCharge_beam.IsChargeStart = true;
		}
		else if (Input->IsMouseReleased(Mouse::MOUSE_LEFT) && pCharge_beam.IsChargeStart) {
			if (!pCharge_beam.IsBlasting)
				pCharge_beam.CancelCharge();
			pCharge_beam.IsChargeStart = false;
		}

		Vec3f temp_dir;
		if (pCharge_beam.IsFiring() || pCharge_beam.CanCloseBeam) {
			Mat3f rotMatrix;
			// Get camera position and direction.
			Vec3f cameraPos = RendererSystem::GetInstance()->GameCamera.tf.GetPosition();
			rotMatrix.SetRotation(RendererSystem::GetInstance()->GameCamera.tf.GetRotation());
			Vec3f cameraDir = rotMatrix * Vec3f::Forward();
			BE::EntityTagFilter filter = EntityTagFilter({ "Wall" }, FilterType::ContainsAny, 0);
			Vec3f camDir_Norm = cameraDir.Normalised();

			Vec3f single_contact;
			EntityID hit_id = PhySys->RayCastSingleHit(single_contact, temp_dir, cameraPos, camDir_Norm * pCharge_beam.cRayLength, filter);

			single_contact = hit_id != NULL_ENTITY ? single_contact : cameraPos + cameraDir * 1000.f;
			Vec3f sourcePos = ECS->GetComponent<Transform>(pCharge_beam.cBeamSource).GetPosition();
			temp_dir = single_contact - sourcePos;
		}
		if (pCharge_beam.UpdateBlasting(temp_dir, dt, true)) {
#ifdef TEEHEE_IS_GOD
			if (baby_monkey) baby_monkey->AddStressOvertime(1.f, pCharge_beam.cBlast_duration);
#else
			if(baby_monkey) baby_monkey->AddStressOvertime(pLaserStress, pCharge_beam.cBlast_duration);
#endif
		};
		pCharge_beam.UpdateEndCharge(temp_dir, dt);
	}

	void PlayerCharacter::ResetLaser()
	{
		pCharge_beam.CancelCharge();
		ECS->GetComponent<AudioSource>(id).Stop(AssetID(10314355125321005166));	//Laser
		//pIsLasering = false;
	}

	inline void PlayerCharacter::BananaSkill_Input(float dt, bool isMoving)
	{
		//Still got banana left
		if (pBananas_current > 0)
		{
			if (!pBanana_shoot.IsOnCoolDown())
			{
				ShootBanana(isMoving);
				idleSFXTimer = idleSFXRange.y;
			}
		}

		//Out of banana
		if (pBananas_current <= 0)
		{
			//Go back to melee, here will set the small icon
			SetCurrentPlayerSkill(PlayerSkills::Melee);

			//Disable banana button
			BE::UI->GetCanvas(bananaButton.first)->GetUI(bananaButton.second)->SetUIElementFlag(BE::UIElementFlags::isDisabled, true);

			//Also disable melee button
			BE::UI->GetCanvas(meleeButton.first)->GetUI(meleeButton.second)->SetUIElementFlag(BE::UIElementFlags::isDisabled, true);
		}
	}

	void PlayerCharacter::AddBananas(uint8_t Add) 
	{
		//If current skill is not banana then enable the button
		if (pCurrent_Equipped_skill != PlayerSkills::Banana)
		{
			BE::UI->GetCanvas(bananaButton.first)->GetUI(bananaButton.second)->SetUIElementFlag(BE::UIElementFlags::isDisabled, false);
		}

		if (pBananas_capacity - pBananas_current < Add)
		{
			pBananas_current = pBananas_capacity;
		}
		else
		{
			pBananas_current += Add;
		}

		//Update the text
		auto text = BE::UI->GetCanvas(bananaCountText.first)->GetUI<BE::UIFontText>(bananaCountText.second);
		text->SetText(std::to_string(pBananas_current) + "/" + std::to_string(pBananas_capacity));
	}

	inline void PlayerCharacter::BabySkill_Update(float dt, bool IsMoving)
	{
		switch (baby_monkey->GetCurrentSkill())
		{
		case BabySkill::Laser: {
			//LaserSkill_Input(dt, IsMoving);
			break;
		}
		case BabySkill::Shield: {
			ShieldSkill_Input(dt);
			break;
		}
		}
	}

	inline void PlayerCharacter::AttackMode_Update(float dt, bool isMoving)
	{
		ADS_mode = false;
		if (pCanAttack && Input->IsMouseTriggered(Mouse::MOUSE_RIGHT)) {
			ToggleADS(true);
		}
		if (pCanAttack && Input->IsMousePressed(Mouse::MOUSE_RIGHT))
		{	
			ADS_mode = true;
		}
		if (!ADS_mode && camera_control->GetIsOnZoomed())
		{
			ToggleADS(false);
		}

		//if(HasBabyMonkey())
		//	BabySkill_Update(dt, isMoving);

		switch (pCurrent_Equipped_skill) {
		case PlayerSkills::Melee: {
			if (pCanAttack && Input->IsMouseTriggered(Mouse::MOUSE_LEFT))
				MeleeSkill_Input(dt);
			break;
		}
		case PlayerSkills::Banana: {
			if (pCanAttack && Input->IsMouseTriggered(Mouse::MOUSE_LEFT))
				BananaSkill_Input(dt, isMoving);
			break;
		}
		case PlayerSkills::Laser: {
			BeamSkill_Input(dt);
			break;
		}
		case PlayerSkills::Shield: {
			ShieldSkill_Input(dt);
			break;
		}
		}
		// if done with shooting banana and ready to throw peel
		//if (Shot_banana && !pPeel_throw.IsOnCoolDown())
		//{
		//	PeelThrow(isMoving);
		//	idleSFXTimer = idleSFXRange.y;
		//}
		// else if left click do attack
		//else if (Input->IsMouseTriggered(Mouse::MOUSE_LEFT) && !(ADS_mode && HasBabyMonkey()))
		//{
		//	if (pBananas_current != 0)
		//	{
		//		if (!pBanana_shoot.IsOnCoolDown())
		//		{
		//			ShootBanana(isMoving);
		//			idleSFXTimer = idleSFXRange.y;
		//		}
		//				
		//	}
		//	else {
		//		if (!pMelee_attack.IsOnCoolDown())
		//		{
		//			MeleeAttack();
		//			idleSFXTimer = idleSFXRange.y;
		//			AudioSource& as = ECS->GetComponent<AudioSource>(id);
		//			as.Play(AssetID(112463090169078739));//TeeHeeAttack.wav
		//		}
		//				
		//	}
		//}
	}

	inline void PlayerCharacter::NannyMode_Update(float dt)
	{
		/*if (Input->IsMousePressed(Mouse::MOUSE_LEFT)) {
			if(baby_monkey)
			baby_monkey->CoddleDecreasingStress(dt);
		}*/
		if (!baby_monkey) return;

		if (pFeedBabyDuration.IsTimerEnded()) {
			SwitchToMode(PlayerMode::Attack);
		}
	}

	inline void PlayerCharacter::Movement_Update(float dt)
	{
		if (CameraController::inst->targetState != CameraTargetState::FollowingPlayer)
		{
			HandleInput(Vec3f(), true, false, false, static_cast<float>(dt));
			return;
		}
		// Determine controller input
		Vec3f control_input = Vec3f();
		if (Input->IsKeyPressed(Key::_W))		control_input += Vec3f::Forward();
		if (Input->IsKeyPressed(Key::_S))		control_input += Vec3f::Backward();
		if (Input->IsKeyPressed(Key::_A))		control_input += Vec3f::Left();
		if (Input->IsKeyPressed(Key::_D))		control_input += Vec3f::Right();
		if (control_input != Vec3f()) {
			control_input = control_input.Normalise();
			auto uiController = ECS->GetComponent<GameLogic>(idGameManager).GetScriptDerived<TutorialUIController>();
			uiController->SetTutorialComplete(TutorialUIController::TutorialIndex::MovementCheckbox);
		}
			
		// Rotate controls
		Mat3f rotMatrix;
		rotMatrix.SetRotation(ECS->GetComponent<Transform>(id).GetRotation());
		control_input = rotMatrix * control_input;

		// Check actions
		bool jump = false;
		bool switch_stance = false;

		//bool running = false;
		/*if (Input->IsKeyPressed(Key::LEFT_SHIFT))
			running = true;*/

		if (Input->IsKeyTriggered(Key::LEFT_CONTROL))
			switch_stance = true;
		else if (Input->IsKeyTriggered(Key::SPACE))
			jump = true;

		HandleInput(control_input, true, jump, switch_stance, static_cast<float>(dt));
	}

	inline void PlayerCharacter::InteractFront()
	{
		std::vector<EntityID> entities_in_AOE;
		float Box_HalfZ = 0.8f;
		Vec3f center = charac_body->GetColliderFrontalPos(Box_HalfZ);
		Vec3f box_halfExtent = Vec3f(Box_HalfZ, 1.2f ,Box_HalfZ);
		PhySys->AABoxCollide(entities_in_AOE, center - box_halfExtent, center + box_halfExtent);

		for (size_t i = 0; i < entities_in_AOE.size(); i++)
		{
			if (ECS->HasComponent<GameLogic>(entities_in_AOE[i])) {
				auto scrpt_ptr = ECS->GetComponent<GameLogic>(entities_in_AOE[i]).GetScript<Interactable>();
				Interactable* intrct_ptr = dynamic_cast<Interactable*>(scrpt_ptr.get());
				if (intrct_ptr != nullptr) {
					intrct_ptr->InteractObject();
				}
			}	
		}
	}

	inline void PlayerCharacter::ToggleBananaCounter(bool show)
	{
		std::shared_ptr<BE::UIFontText> bananaCountT = BE::UI->GetCanvas(bananaCountText.first)->GetUI<BE::UIFontText>(bananaCountText.second);
		if (bananaCountT)
			bananaCountT->SetUIElementFlag(UIElementFlags::isActive, show);
		std::shared_ptr<BE::UIImage> bananaCountbg = BE::UI->GetCanvas(bananaCountBG.first)->GetUI<BE::UIImage>(bananaCountBG.second);
		if (bananaCountbg) bananaCountbg->SetUIElementFlag(UIElementFlags::isActive, show);
	}

	inline Vec3f PlayerCharacter::GetCrosshairContactPosition()
	{
		Vec3f contact_pt, contact_norm;
		Mat3f rotMatrix;

		// Get camera position and direction.
		Vec3f cameraPos = RendererSystem::GetInstance()->GameCamera.tf.GetPosition();
		rotMatrix.SetRotation(RendererSystem::GetInstance()->GameCamera.tf.GetRotation());
		Vec3f cameraDir = rotMatrix * Vec3f::Forward();
		Vec3f cameraDir_Norm = cameraDir.Normalised() * 1000.f;

		EntityID hit_id = PhySys->RayCastSingleHit(contact_pt, contact_norm, cameraPos, cameraDir_Norm, EntityTagFilter(0));

		Vec3f first_contact = hit_id != NULL_ENTITY ? contact_pt : cameraPos + cameraDir * 1000.f;
		return first_contact;
	}


	void PlayerCharacter::Update(double dt)
	{
		if (!initialDialogueCompleted && !PauseControl::GetPause())
		{
			if (Input->IsKeyTriggered(Key::_0))//HACK TO SKIP INITAL CUTSCENE
			{
				initialDialogueCompleted = true;
			}
			if (!intialCutSceneStarted)
			{
				EntityID m_cdoController = BE::ECS->FindEntitiesByName("Dialogue_Cutscene_Objectives_Controller", 1).front();
				BE::GameLogic& gl = BE::ECS->GetComponent<BE::GameLogic>(m_cdoController);
				std::shared_ptr<CutsceneController> ptr = gl.GetScriptDerived<CutsceneController>();
				ptr->StartCutscene(0);
				intialCutSceneStarted = true;
				return;
			}
			if (!initialDialogueStarted)
			{
				
				intialDialogueDelay -= static_cast<float>(dt);
				if (intialDialogueDelay < 0.0f)
				{
					DialogueController::inst->StartDialogue(0);
					initialDialogueStarted = true;
				}
				return;
			}
			else if (!DialogueController::inst->IsDialogueShowing())
			{
				initialDialogueCompleted = true;
				CameraController::inst->ShowControlScreen();
			}
			else
				return;
		}
		if (charac_base->IsDead()) 
		{
			SetAnimState(PlayerAnim::Death, true);
			charac_body->cCharacter->SetLinearVelocity(JPH::Vec3(0.f, 0.f, 0.f));
			if (baby_monkey)
				RemoveBabyMonkey();
			auto ptr = ECS->GetComponent<GameLogic>(idGameManager).GetScript<LevelManager>();
			dynamic_cast<LevelManager*>(ptr.get())->ShowGameOver();
			return;
		}
		float dtime = static_cast<float>(dt);
		UpdateTimers(dtime);
		AutoResetAnimation(dtime);

		//std::cout << "blasting:" << pCharge_beam.IsBlasting << " | extending: " << pCharge_beam.IsExtending << " | Canclosebeam: " << pCharge_beam.CanCloseBeam << "\n";
		// switching player mode
		if (pCanSwitchMode && pModeSwitch.IsCooldownEnded() && !pCharge_beam.IsInAnimation())
		{
			if (pPlayerMode == PlayerMode::Attack) {
				SwitchToMode(PlayerMode::Nanny);
			}
			else if (pPlayerMode == PlayerMode::Nanny) {
				SwitchToMode(PlayerMode::Attack);
			}
		}
		// tp player
		if (Input->IsKeyReleased(Key::_M)) {
			ECS->GetComponent<Transform>(id).SetPosition(Vec3f(82.0f, -16.f, -169.f));
			ECS->GetComponent<Transform>(id).SetRotation(Quatf(0.0f, 0.703f, 0.f, 0.71089f));
				//"x": 82.01781463623047,
				//"y" : -16.645580291748047,
				//"z" : -169.7923583984375

			//"i": 0.0,
			//	"j" : 0.7032951712608337,
			//	"k" : 0.0,
			//	"r" : 0.7108980417251587
		}
		// switching 
		//if (Input->IsKeyPressed(Key::_1))			SetCurrentPlayerSkill(PlayerSkills::Melee);
		//else if (Input->IsKeyPressed(Key::_2))		SetCurrentPlayerSkill(PlayerSkills::Banana);
		//else if (Input->IsKeyPressed(Key::_3))		SetCurrentPlayerSkill(PlayerSkills::Laser);
		//else if (Input->IsKeyPressed(Key::_4))		SetCurrentPlayerSkill(PlayerSkills::Shield);

		// allow other input if player is not in ADS_mode nor is halfway switching mode.
		if (!pModeSwitch.IsOnCoolDown())
		{
			if (Input->IsKeyTriggered(Key::_Q) && !pCharge_beam.IsInAnimation())
			{
				pModeSwitch.start = true;
			}
			else if (Input->IsKeyTriggered(Key::_E))
			{
				if (!pIsPlayingOneShot && player_interact)
					player_interact->TriggerInteractObject();
			}
		}

		////debug
		//if (Input->IsKeyTriggered(Key::TAB) && HasBabyMonkey())
		//	std::cout << "Stress: " << baby_monkey->GetCurrentStress() << "\n";

		Movement_Update(dtime);

		bool IsMovingThisFrame = false;
		auto velocity = charac_body->cCharacter->GetLinearVelocity();
		if (velocity.GetX() * velocity.GetX() + velocity.GetZ() * velocity.GetZ() > 0.9f) {
			IsMovingThisFrame = true;
		}

		if (!pModeSwitch.IsOnCoolDown()) {
			// update controls input depending on player mode
			switch (pPlayerMode) {
			case PlayerMode::Attack: {
				AttackMode_Update(dtime, IsMovingThisFrame);
				break;
			}
			case PlayerMode::Nanny: {
				NannyMode_Update(dtime);
				break;
			}
			}
		}
		//std::cout << static_cast<int>(pPlayerMode) << "\n";

		if (!pIsPlayingOneShot && pPlayerMode != PlayerMode::Nanny) {
			if (IsBabySkill(GetCurrentPlayerSkill()))
				SetAnimState(PlayerAnim::RunBaby);
			else if (IsMovingThisFrame) {
					SetAnimState(PlayerAnim::Run);
			}		
			else
				SetAnimState(PlayerAnim::Idle);
		}
		else if (pPlayerMode == PlayerMode::Nanny){
			SetAnimState(PlayerAnim::RunBaby);
		}
		if(DialogueController::inst == nullptr || !DialogueController::inst->IsDialogueShowing())
			idleSFXTimer -= static_cast<float>(dt);
		if (idleSFXTimer < 0)
		{
			AudioSource& as = ECS->GetComponent<AudioSource>(id);
			//as.SetVolume(0.03f);
			as.Play(idleSFXID[Rand::Random<size_t>(0, idleSFXID.size() - 1)]);
			idleSFXTimer = Rand::Random(idleSFXRange.x, idleSFXRange.y);
		}
	}

	void PlayerCharacter::Cleanup()
	{
	}
	void PlayerCharacter::UponSleep()
	{

	}
	void PlayerCharacter::UponWake()
	{

	}
	void PlayerCharacter::OnCollision(EntityID collidingID)
	{
		//std::cout << "Collided entity: " << ECS->GetComponent<EntityInfo>(collidingID).GetName() << "\n";
	}
	void PlayerCharacter::OnPersistedCollision(EntityID collidingID)
	{

	}
	void PlayerCharacter::OnCollisionRemoved(EntityID collidingID)
	{

	}

	void PlayerCharacter::InitAnimationInfo(EntityID modelID)
	{
		if (ECS->HasComponent<AnimationComponent>(modelID) && ECS->HasComponent<Renderer>(modelID))
		{
			//Check if model is loaded
			if (AssetMgr->IsAssetDataLoaded(ECS->GetComponent<Renderer>(modelID).getModelID()))
			{
				//now go through all animations
				auto& vector = AssetMgr->GetAssetData<BeModel>(ECS->GetComponent<Renderer>(modelID).getModelID()).GetAnimationDetails();
				for (size_t i{6}; i < vector.size(); ++i)
				{
					if (i - 6 >= pAnim_Indexes.size()) break;
					//std::cout << vector[i].GetCurrentAnimationName() << "\n";
					if (pAnim_Indexes[i - 6].index < 0) {
						pAnim_Indexes[i - 6].index = (int)i;
						pAnim_Indexes[i - 6].duration = vector[i].GetDuration() / vector[i].GetTicksPerSecond();
					}
				}
			}
		}
	}

	void PlayerCharacter::AutoResetAnimation(float dt)
	{
		HasSetAnimation_frame = false;
		if (!pIsPlayingOneShot) return;

		if (ECS->GetComponent<AnimationComponent>(pModelID).getCompletedOnce()) {
			pIsPlayingOneShot = false;
			SetAnimState(PlayerAnim::Idle);
		}
	}

	void PlayerCharacter::SetAnimState(PlayerAnim in, bool force)
	{
		size_t index = static_cast<size_t>(in);
		// Check if animation was found and loaded.
		if (pAnim_Indexes[index].index < 0) return;
		if (in == pCurrent_Anim) return;

		if (!force) {
			if(pIsPlayingOneShot || HasSetAnimation_frame) return;
			else pPrev_Anim = pCurrent_Anim;
		}
		else {
			if (pIsPlayingOneShot)
				pIsPlayingOneShot = false;
		}

		pCurrent_Anim = in;
		ECS->GetComponent<AnimationComponent>(pModelID).SetAnimation(pAnim_Indexes[index].index);
		HasSetAnimation_frame = true;

		ECS->GetComponent<AnimationComponent>(pModelID).setTickFactor(1.f);
		ECS->GetComponent<AnimationComponent>(pModelID).setFreezeAtLastFrame(false);
		switch (in) {
		case PlayerAnim::Idle: {
			break;
		}
		case PlayerAnim::InteractCrate: {
			pIsPlayingOneShot = true;
			break;
		}
		case PlayerAnim::Jump: {		
			ECS->GetComponent<AnimationComponent>(pModelID).setTickFactor(2.0f);
			pIsPlayingOneShot = true;
			break;
		}
		case PlayerAnim::Melee: {
			ECS->GetComponent<AnimationComponent>(pModelID).setTickFactor(1.8f);
			pIsPlayingOneShot = true;
			break;
		}
		case PlayerAnim::Death: {
			ECS->GetComponent<AnimationComponent>(pModelID).setFreezeAtLastFrame(true);
			break;
		}
		case PlayerAnim::PickUpBaby: {
			pIsPlayingOneShot = true;
			break;
		}
		case PlayerAnim::Run: {
			ECS->GetComponent<AnimationComponent>(pModelID).setTickFactor(1.8f);
			break;
		}
		case PlayerAnim::RunShoot: {
			ECS->GetComponent<AnimationComponent>(pModelID).setTickFactor(1.2f);
			pIsPlayingOneShot = true;
			break;
		}
		case PlayerAnim::RunBaby: {
			ECS->GetComponent<AnimationComponent>(pModelID).setTickFactor(1.8f);
			break;
		}
		case PlayerAnim::StandFire: {
			ECS->GetComponent<AnimationComponent>(pModelID).setTickFactor(1.2f);
			pIsPlayingOneShot = true;
			break;
		}
		/*case PlayerAnim::StandReload: {
			ECS->GetComponent<AnimationComponent>(pModelID).setTickFactor(1.8f);
			pIsPlayingOneShot = true;
			break;
		}*/
		default: {
			break;
		}
		}
	}

	void PlayerCharacter::SetCurrentPlayerSkill(PlayerSkills change)
	{
		if (pCurrent_Equipped_skill == change) return;

		if (IsBabySkill(change)) {
			if (!HasBabyMonkey()) return;
			if (pPlayerMode != PlayerMode::Nanny) baby_monkey->SetBabyToAttackPosition();
		}
		else if (IsBabySkill(pCurrent_Equipped_skill)) {
			ResetLaser();
			if (pPlayerMode != PlayerMode::Nanny) baby_monkey->SetBabyToCarryPosition();
		}

		// hide banana counter
		if (pCurrent_Equipped_skill == PlayerSkills::Banana) {
			ToggleBananaCounter(false);
		}


		pCurrent_Equipped_skill = change;

		std::shared_ptr<BE::UIImage> image = BE::UI->GetCanvas(abilityIcon.first)->GetUI<BE::UIImage>(abilityIcon.second);
		if (image) 
		{
			switch (pCurrent_Equipped_skill) 
			{
			case PlayerSkills::Melee: {
				image->SetTexture(BEID(16754810366861521257));		//MeleeAbilityIcon.dds
				break;
			}
			case PlayerSkills::Banana: {
				image->SetTexture(BEID(11465357449148116611));		//BananaAbilityIcon.dds
				ToggleBananaCounter(true);				
				break;
			}
			case PlayerSkills::Laser: {
				image->SetTexture(BEID(13120178699040930271));		//LaserEyes_Icon.dds
				break;
			}
			case PlayerSkills::Shield: {
				image->SetTexture(BEID(5908635313167515298));		//Shield_Icon.dds
				break;
			}
			}
		}


		auto uiController = ECS->GetComponent<GameLogic>(idGameManager).GetScriptDerived<TutorialUIController>();
		uiController->SetTutorialComplete(TutorialUIController::TutorialIndex::ChangeWeaponCheckBox);
	}
	

	void PlayerCharacter::ToggleCrosshair(bool show) 
	{
		BE::UI->GetCanvas(crosshairTexture.first)->GetUI<BE::UIImage>(crosshairTexture.second)->SetUIElementFlag(UIElementFlags::isActive, show);
	}

	void PlayerCharacter::RemoveBabyMonkey()
	{
		Log("removed BabyMonkey");
		if (IsBabySkill(pCurrent_Equipped_skill))
		{
			//Go back to melee, here will set the small icon
			SetCurrentPlayerSkill(PlayerSkills::Melee);

			//Disable baby button
			BE::UI->GetCanvas(laserButton.first)->GetUI(laserButton.second)->SetUIElementFlag(BE::UIElementFlags::isDisabled, true);

			//Also disable melee button
			BE::UI->GetCanvas(meleeButton.first)->GetUI(meleeButton.second)->SetUIElementFlag(BE::UIElementFlags::isDisabled, true);

			//Show the baby pointer canvas
			BE::CanvasID babyPointerCanvas = BE::UI->FindUICanvasesByName("BabyPointer", 1).front();
			BE::UI->GetCanvas(babyPointerCanvas)->SetIsActive(true);
		}
		if (pPlayerMode == PlayerMode::Nanny) {
			SwitchToMode(PlayerMode::Attack);
		}
		if (baby_monkey) baby_monkey->RemoveFromParent();
		baby_monkey = nullptr;
	}

	void PlayerCharacter::ResurretPlayer(const Vec3f& _pos, const Vec3f& _rot)
	{
		//TP player back to the checkpoint
		BE::ECS->GetComponent<BE::Transform>(BE::GLS->GetPlayerID()).SetPosition(_pos);
		BE::ECS->GetComponent<BE::Transform>(BE::GLS->GetPlayerID()).SetEulerRotation(_rot);


		//Restore the HP to full
		auto charScript = BE::ECS->GetComponent<BE::GameLogic>(id).GetScriptDerived<CharacterScript>();
		charScript->SetCurrentHealth(charScript->GetTotalHealth());
		charScript->DamageHealth(0);

		//Set to melee
		SetCurrentPlayerSkill(PlayerSkills::Melee);

		//Set the Ability wheel UI
		BE::UI->GetCanvas(meleeButton.first)->GetUI(meleeButton.second)->SetUIElementFlag(BE::UIElementFlags::isDisabled, true);

		//Only enable banana button if there is still bananas remaining
		BE::UI->GetCanvas(bananaButton.first)->GetUI(bananaButton.second)->SetUIElementFlag(BE::UIElementFlags::isDisabled, pBananas_current <= 0);

		//Only enable lase button if carrying baby
		BE::UI->GetCanvas(laserButton.first)->GetUI(laserButton.second)->SetUIElementFlag(BE::UIElementFlags::isDisabled, !HasBabyMonkey());
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	rttr::registration::enumeration<PlayerSkills>(RegistrationNameCreator<PlayerSkills>())
		(
			value("Melee", PlayerSkills::Melee),
			value("Laser", PlayerSkills::Laser),
			value("Banana", PlayerSkills::Banana),
			value("Shield", PlayerSkills::Shield)
		);
	registration::class_<PlayerCharacter>(RegistrationNameCreator<PlayerCharacter>())
		.constructor()
		.property("Basic Melee Attack", &PlayerCharacter::pMelee_attack)
		//.property("Baby Laser Attack", &PlayerCharacter::pLaser_attack)
		.property("Banana Shot", &PlayerCharacter::pBanana_shoot)
		.property("Banana Peel Throw", &PlayerCharacter::pPeel_throw)
		.property("Running Speed Multiplier", &PlayerCharacter::pRunning_Multplr);
}


// 	void PlayerCharacter::Init()
// 
//float melee_Damage = 2.f;
//float melee_CD = 1.2f;
//pMelee_attack = AttackSkill(melee_CD, melee_Damage, Vec3f(1.5f, 3.5f, 1.5f), AttackSkill::AOEType::AABox);
//
//float laser_Damage = 0.08f;
//float laser_CD = 1.0f;
//float laser_Charge = 1.0f;
//pLaser_attack = RayCastSkill(laser_CD, laser_Damage, laser_Charge, 20.f, RayCastSkill::RayType::LineRay);

/*	float banana_Damage = 3.0f;
	float banana_CD = 1.0f;
	float banana_speed = 30.0f;
	pBanana_shoot = ProjectileSkill(banana_CD, banana_Damage, 0.25f, banana_speed, ProjectileSkill::ProjectileType::Straight, "p_Banana", 20.f);*/

	/*float peel_stun_duration = 3.0f;
	pPeel_throw = ProjectileSkill(0.5f, 0.5f, 0.4f, 10.f, ProjectileSkill::ProjectileType::Weighted, "p_BananaPeel", 20.f, StatusType::Stun, peel_stun_duration, false);
	pPeel_throw.SetProjectileShape(Shape::Box);*/

	// Stashed Laser attack
	/*
	inline void PlayerCharacter::LaserAttack(float dt)
	{
		pIsLasering = true;
		int numberOfSteps = Engine->GetPhysSteps();
		//fixed update 
		for (int step = 0; step < numberOfSteps; ++step)
		{
			Vec3f eyespos = baby_monkey->GetEyesPosition();
			if (pLaser_attack.rCharge.isCharged)
			{
				std::vector<Vec3f> contact_pts;
				Mat3f rotMatrix;

				// Get camera position and direction.
				Vec3f cameraPos = RendererSystem::GetInstance()->GameCamera.tf.GetPosition();
				rotMatrix.SetRotation(RendererSystem::GetInstance()->GameCamera.tf.GetRotation());
				Vec3f cameraDir = rotMatrix * Vec3f::Forward();
				BE::EntityTagFilter filter = EntityTagFilter(0);
				Vec3f camDir_Norm = cameraDir.Normalised();

				Vec3f single_contact, temp_dir;
				EntityID hit_id = PhySys->RayCastSingleHit(single_contact, temp_dir, cameraPos, camDir_Norm * pLaser_attack.rRayLength, filter);

				single_contact = hit_id != NULL_ENTITY ? single_contact : cameraPos + cameraDir * 1000.f;

				// cast the laser skill and get the first contact point.
				pLaser_attack.CastSkill(id, eyespos, single_contact - eyespos, charac_base->GetBaseAttack(), contact_pts, filter);

				// draw the laser from the first contact point to the player's eyes.
				single_contact = !contact_pts.empty() ? contact_pts.front() : single_contact;
				temp_dir = single_contact - eyespos;

				ECS->GetComponent<EntityInfo>(pLaserID).SetFlag(EntityFlag::active, true);
				AudioSource& as = ECS->GetComponent<AudioSource>(id);

				if (!as.IsAudioPlaying(AssetID(10314355125321005166)))
					as.Play(AssetID(10314355125321005166));				//BabyLaser.wav

				// set laser size, position and rotation
				ECS->GetComponent<Transform>(pLaserID).SetPosition(eyespos + temp_dir * 0.5f);
				ECS->GetComponent<Transform>(pLaserID).SetScale({ 0.1f, 0.1f, temp_dir.Mag() });
				Quatf dir_rot = Vec3f::Forward().get_rotation_between(temp_dir);
				ECS->GetComponent<Transform>(pLaserID).SetRotation(dir_rot);

				baby_monkey->AddFixedLaserStress();
			}
			ECS->GetComponent<EntityInfo>(pLaserLightID).SetFlag(EntityFlag::active, true);
			ECS->GetComponent<Transform>(pLaserLightID).SetPosition(eyespos);
		}

		if (!pLaser_attack.rCharge.isCharged)
		{
			pLaser_attack.rCharge.update_Charging(dt);
			lightComponent& lightcomp = ECS->GetComponent<lightComponent>(pLaserLightID);
			if (lightcomp.getIntensity() < pLaser_intensity_max) {
				float lightrate = pLaser_intensity_rate * dt;
				if (pLaser_intensity_max - lightcomp.getIntensity() > lightrate)
					lightcomp.setIntensity(lightcomp.getIntensity() + lightrate);
				else
					lightcomp.setIntensity(pLaser_intensity_max);
			}
		}
	}
	

	void PlayerCharacter::ResetLaser()
	{
		ECS->GetComponent<EntityInfo>(pLaserID).SetFlag(EntityFlag::active, false);
		ECS->GetComponent<Transform>(pLaserLightID).SetPosition({ 1000.f, 1000.f ,1000.f });
		ECS->GetComponent<EntityInfo>(pLaserLightID).SetFlag(EntityFlag::active, false);
		ECS->GetComponent<lightComponent>(pLaserLightID).setIntensity(0.f);
		pLaser_attack.rCharge.reset_Charge();
		ECS->GetComponent<AudioSource>(id).Stop(AssetID(10314355125321005166));	//Laser
		pIsLasering = false;
	}

	inline void PlayerCharacter::LaserSkill_Input(float dt, bool isMoving)
	{
		//if (Input->IsMousePressed(Mouse::MOUSE_RIGHT))
		//{
		//	baby_monkey->SetBabyToAttackPosition();
		//}
		//if (Input->IsMouseReleased(Mouse::MOUSE_RIGHT))
		//{
		//	ResetLaser();
		//	baby_monkey->SetBabyToCarryPosition();
		//}

		// attacks input
		if (!baby_monkey->IsGoingExplode())
		{
			if ((pCanAttack || pIsLasering) && Input->IsMousePressed(Mouse::MOUSE_LEFT))
			{
				LaserAttack(dt);
			}
			if (Input->IsMouseReleased(Mouse::MOUSE_LEFT)) {
				ResetLaser();
			}
		}
		if (baby_monkey->IsGoingExplode() && pIsLasering) {
			ResetLaser();
		}
	}

	*/

	/*
	const auto results = ECS->FindEntitiesByName("Laser", 1);
	if (results.empty())
	{
		pLaserID = ECS->CreateEntity();
		ECS->GetComponent<EntityInfo>(pLaserID).SetName("Laser");
		ECS->GetComponent<Transform>(pLaserID).SetScale({ 0.1f, 0.1f, 100.f });
		ECS->AddComponent<Renderer>(pLaserID, Renderer());
		ECS->GetComponent<Renderer>(pLaserID).setModelIDwithName("../Assets/Models/Primitives/Cube.bananaModel");
		ECS->GetComponent<Renderer>(pLaserID).setAlbedoAlphaMapwithName("../Assets/Textures/misc/red.dds", 0);
		ECS->GetComponent<EntityInfo>(pLaserID).SetFlag(EntityFlag::active, false);
	}
	else
		pLaserID = results.front();

	const auto light = ECS->FindEntitiesByName("LaserLight", 1);
	if (light.empty())
	{
		pLaserLightID = ECS->CreateEntity();
		ECS->GetComponent<EntityInfo>(pLaserLightID).SetName("LaserLight");
		ECS->GetComponent<Transform>(pLaserLightID).SetScale({ 0.5f, 0.5f, 0.5f });
		ECS->AddComponent<lightComponent>(pLaserLightID);
		ECS->GetComponent<Transform>(pLaserLightID).SetPosition({ 1000.f, 1000.f ,1000.f });
		ECS->GetComponent<lightComponent>(pLaserLightID).setColor({ 1.f, 0.f ,0.f });
		ECS->GetComponent<lightComponent>(pLaserLightID).setIntensity(0.f);
		ECS->GetComponent<lightComponent>(pLaserLightID).setRadius(0.1f);
		ECS->GetComponent<EntityInfo>(pLaserLightID).SetFlag(EntityFlag::active, false);
	}
	else
		pLaserLightID = light.front();
	*/

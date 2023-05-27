#include "CameraController.h"
#include "../Player/PlayerScript.h"
#include "../../UserInterface/InGameUIScripts/InGameUIScripts.h"

namespace TH
{
	using namespace BE;
	BEID target = NULL_ENTITY;
	//Edit this value to change the speed of the camera rotation
	const float mouseRotationSpeed = 0.25f;
	const float maxZoom = -3.f;
	const float minZoom = -4.f;
	const float ADS_Zoom = -1.5f;
	const float fadeInTimer = 3.0f;
	float viewTimer = 1.5f;
	const float fadeOutTimer = 3.0f;
	Vec3f originalPos;
	Vec3f zoomedPos;
	Vec3f facingTarget;
	Vec3f targetingCameraRotation;
	Vec3f targetingCameraPosition;
	Quatf originalRotation;
	const Vec3f headOffset = (0.0f,0.75f,0.0f);
	std::function<void(float)> viewTriggerFunction;
	CameraController* CameraController::inst;
	std::pair<CanvasID, UIElementID> blackCanvas;
	void CameraController::FollowPlayerState(float dt)
	{
		BE::Transform& transf = BE::ECS->GetComponent<BE::Transform>(id);
		BE::Vec3f currlocalPos = transf.GetLocalPosition();
		if (AllowZoom) {
			if (BE::Input->GetVerticalScroll() > 0)
			{
				currlocalPos += BE::Vec3f::Forward();
				currlocalPos.z = std::max(std::min(currlocalPos.z, maxZoom), minZoom);
				transf.SetLocalPosition(currlocalPos);
			}
			else if (BE::Input->GetVerticalScroll() < 0)
			{
				currlocalPos -= BE::Vec3f::Forward();
				currlocalPos.z = std::max(std::min(currlocalPos.z, maxZoom), minZoom);
				transf.SetLocalPosition(currlocalPos - BE::Vec3f::Forward());
			}
		}

		
		auto scriptptr = ECS->GetComponent<GameLogic>(target).GetScript<PlayerCharacter>();
		PlayerCharacter& player_ptr = *dynamic_cast<PlayerCharacter*>(scriptptr.get());
		if (!BE::Input->IsCursorEnabled() && player_ptr.initialDialogueCompleted)
		{
			Vec3f eulerAngler = ECS->GetComponent<Transform>(target).GetEulerRotation();
			eulerAngler.y += std::min(BE::Input->GetMouseDeltaX(), 50.0f) * mouseRotationSpeed;
			player_ptr.RotateCharacter(eulerAngler);
			if (AllowVertical) {
				const BEID& parent = ECS->GetComponent<Transform>(id).GetParent();
				eulerAngler = ECS->GetComponent<Transform>(parent).GetLocalEulerRotation();
				eulerAngler.y = 0.0f;
				Vec3f oldEuler = eulerAngler;
				eulerAngler.x += BE::Input->GetMouseDeltaY() * mouseRotationSpeed;
				eulerAngler.x = std::min(std::max(-45.0f, eulerAngler.x), 45.0f);
				ECS->GetComponent<Transform>(parent).SetLocalEulerRotation(eulerAngler);
			}
		}
		const Vec3f targetTransHead = ECS->GetComponent<Transform>(target).GetPosition() + headOffset;
		Vec3f contactPoint, contactNorm;
		Vec3f playerToCameraDirection = (transf.GetPosition() - targetTransHead);
		RendererSystem::GetInstance()->GameCamera.tf = transf;
		if (PhySys->RayCastSingleHit(contactPoint, contactNorm, targetTransHead
			, playerToCameraDirection * 1.25f,
			EntityTagFilter({ "Enemy","CameraIgnore"}, BE::FilterType::DoesNotContainAny, 0, target)) != NULL_ENTITY)
		{
			RendererSystem::GetInstance()->GameCamera.tf.SetPosition(contactPoint - playerToCameraDirection * 0.25f);
		}
	}
	void CameraController::TargetingState(float dt)
	{
		timer += dt;
		float duration = timer;
		if (timer < fadeInTimer)
		{
			float interpolated = duration / fadeInTimer;
			if (interpolated > 0.5f)
			{
				RendererSystem::GetInstance()->GameCamera.tf.SetPosition(targetingCameraPosition);
				RendererSystem::GetInstance()->GameCamera.tf.SetEulerRotation(targetingCameraRotation);
				UI->GetUI<UIImage>(blackCanvas.first, blackCanvas.second)->setOpacity(1 - (interpolated - 0.5f) * 2);
				//Set opacity using 1 - (interpolated - 0.5f) * 2; 
			}
			else
			{
				//Set opacity using interpolated * 2;  
				UI->GetUI<UIImage>(blackCanvas.first, blackCanvas.second)->setOpacity(interpolated * 2);
				RendererSystem::GetInstance()->GameCamera.tf = BE::ECS->GetComponent<BE::Transform>(id);
			}
		}
		else
		{
			duration -= fadeInTimer;
			if (duration < viewTimer)
			{
				viewTriggerFunction(duration/viewTimer);
			}
			else
			{
				duration -= viewTimer;
				if (duration < fadeOutTimer)
				{
					float interpolated = duration / fadeOutTimer;
					if (interpolated > 0.5f)
					{
						//Set opacity using 1 - (interpolated - 0.5f) * 2;
						TurnToFace(facingTarget);
						RendererSystem::GetInstance()->GameCamera.tf = BE::ECS->GetComponent<BE::Transform>(id);
						UI->GetUI<UIImage>(blackCanvas.first, blackCanvas.second)->setOpacity(1 - (interpolated - 0.5f) * 2);
					}
					else
					{
						//Set opacity using interpolated * 2;
						UI->GetUI<UIImage>(blackCanvas.first, blackCanvas.second)->setOpacity(interpolated * 2);
					}
				}
				else
				{
					UI->GetUI<UIImage>(blackCanvas.first, blackCanvas.second)->setOpacity(0.0f);
					UI->GetCanvas(blackCanvas.first)->SetIsActive(false);
					targetState = CameraTargetState::FollowingPlayer;
				}
			}
		}
	}
	void CameraController::SetTarget(Vec3f _target,Vec3f cameraPos, Vec3f cameraRot, float _viewTimer, std::function<void(float)> function)
	{
		timer = 0.0f;
		targetState = CameraTargetState::Targeting;
		facingTarget = _target;
		targetingCameraPosition = cameraPos;
		targetingCameraRotation = cameraRot;
		viewTimer = _viewTimer;
		viewTriggerFunction = function;
		UI->GetCanvas(blackCanvas.first)->SetIsActive(true);
	}
	void CameraController::Awake()
	{
		inst = this;
		blackCanvas = BE::UI->FindUIElementByName("CutsceneBG", 1).front();
		pauseCanvas = BE::UI->FindUICanvasesByName("PauseCanvas", 1).front();
		controlCanvas = BE::UI->FindUICanvasesByName("Controls_Page", 1).front();
		gameMgr = BE::ECS->FindEntitiesByName("GameManager", 1).front();
	}
	void CameraController::Init()
	{
		 const auto results = ECS->FindEntitiesByName("TeeHee", 1);
		 if (results.size() != 1)
		 {
			 LogCoreError("Error finding teehee to attach to");
		 }
		 else
		 {
			 target = results.front();
		 }
		 BE::Input->SetEnableCursor(false);
		 originalPos = BE::ECS->GetComponent<BE::Transform>(id).GetLocalPosition();
		 //Edit zoomed pos or ontop, setting here so that i can use the values from the original pos
		 zoomedPos = originalPos;
		 zoomedPos.z = ADS_Zoom;
	}

	void CameraController::TurnToFace(Vec3f pos)
	{
		Transform& tf = ECS->GetComponent<Transform>(target);
		const auto dir = pos - tf.GetPosition();
		float angle = atan2f(dir.z, -dir.x) * 180.0f/static_cast<float>(Math::PI);
		auto scriptptr = ECS->GetComponent<GameLogic>(target).GetScript<PlayerCharacter>();
		PlayerCharacter& player_ptr = *dynamic_cast<PlayerCharacter*>(scriptptr.get());
		player_ptr.RotateCharacter(Vec3f(0, angle - 90, 0));
	}
	void CameraController::Update(double dt)
	{
 		if (BE::Input->IsKeyTriggered(BE::Key::ESCAPE))
		{
			SetPause();
		}
		switch (targetState)
		{
		case CameraTargetState::FollowingPlayer:
			FollowPlayerState(static_cast<float>(dt));
			break;
		case CameraTargetState::Targeting:
			TargetingState(static_cast<float>(dt));
			break;
		default:
			break;
		}
	}

	//void CameraController::SetMouseState(bool in)
	//{
	//	//mouseState = in;
	//	BE::Input->SetEnableCursor(in);

	//	/*if (in)
	//		glfwSetInputMode(RendererSystem::GetInstance()->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//	else
	//		glfwSetInputMode(RendererSystem::GetInstance()->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);*/
	//}

	void CameraController::ZoomCamera_ADS()
	{
		BE::Transform& transf = BE::ECS->GetComponent<BE::Transform>(id);
		originalPos = transf.GetLocalPosition();
		transf.SetLocalPosition(zoomedPos);
		RendererSystem::GetInstance()->GameCamera.tf = transf;
		isOnZoomed = true;
		AllowZoom = false;
	}
	void CameraController::ResetZoomCamera_ADS()
	{
		BE::Transform& transf = BE::ECS->GetComponent<BE::Transform>(id);
		transf.SetLocalPosition(originalPos);
		RendererSystem::GetInstance()->GameCamera.tf = transf;
		isOnZoomed = false;
		AllowZoom = true;
	}
	void CameraController::SetCameraPosition_TopDown()
	{
		BE::Transform& transf = BE::ECS->GetComponent<BE::Transform>(id);
		originalPos = transf.GetLocalPosition();
		transf.SetLocalPosition(TopDown_pos);

		const BEID& parent = ECS->GetComponent<Transform>(id).GetParent();
		original_rot = ECS->GetComponent<Transform>(parent).GetLocalEulerRotation();
		ECS->GetComponent<Transform>(parent).SetLocalEulerRotation(TopDown_rot);

		RendererSystem::GetInstance()->GameCamera.tf = transf;
	}
	void CameraController::SetCameraPosition_Original()
	{
		BE::Transform& transf = BE::ECS->GetComponent<BE::Transform>(id);
		transf.SetLocalPosition(originalPos);

		const BEID& parent = ECS->GetComponent<Transform>(id).GetParent();
		ECS->GetComponent<Transform>(parent).SetLocalEulerRotation(original_rot);

		RendererSystem::GetInstance()->GameCamera.tf = transf;
	}
	void CameraController::SetPause()
	{
		//SetMouseState(true);
		BE::Input->SetEnableCursor(true);
		BE::UI->GetCanvas(pauseCanvas)->SetIsActive(true);
		BE::PauseControl::SetPause(true);
		
	}
	void CameraController::ShowControlScreen()
	{
		BE::Input->SetEnableCursor(true);

		BE::UI->GetCanvas(controlCanvas)->SetIsActive(true);
		BE::PauseControl::SetPause(true);
		BE::UI->SaveFontState();
		
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<CameraController>(RegistrationNameCreator<CameraController>())
		.constructor();
}

#pragma once
#include <BananaEngine.h>
namespace TH
{
	using namespace BE;
	enum class CameraTargetState
	{
		FollowingPlayer,
		Targeting
	};
	class CameraController : public  IScript
	{
		Vec3f TopDown_pos = Vec3f(0.75f, 1.f, -5.2f);
		Vec3f TopDown_rot = Vec3f(30.f, 0.f, 0.f);
		Vec3f original_rot;
		float timer = 0.0f;
		bool AllowZoom = true;
		bool AllowVertical = true;
		//bool mouseState;
		bool isOnZoomed = false;
		bool cinematicMode = false;
		void FollowPlayerState(float dt);
		void TargetingState(float dt);

		//Canvases
		BE::CanvasID pauseCanvas = BE::NULL_UICANVAS;
		BE::CanvasID controlCanvas = BE::NULL_UICANVAS;

		//Game manager
		BE::EntityID gameMgr = BE::NULL_ENTITY;

	public:
		CameraTargetState targetState;
		void SetTarget(Vec3f target,Vec3f cameraPos, Vec3f cameraRot, float viewTimer, std::function<void(float)> function);
		static CameraController* inst;
		void Awake();
		void Init();
		void TurnToFace(Vec3f pos);
		void Update(double dt);
		void ZoomCamera_ADS();
		void ResetZoomCamera_ADS();
		void SetCameraPosition_TopDown();
		void SetCameraPosition_Original();
		void SetPause();
		void ShowControlScreen();
		inline void SetAllowZoom(bool in) { AllowZoom = in; }
		inline void SetAllowVertical(bool in) { AllowVertical = in; }
		//void SetMouseState(bool in);
		//inline bool GetMouseState() const { return mouseState; }
		inline bool GetIsOnZoomed() const { return isOnZoomed; }
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};
}
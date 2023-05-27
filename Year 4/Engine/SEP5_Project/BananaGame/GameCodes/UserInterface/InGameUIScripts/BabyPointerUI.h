#pragma once
#include <BananaEngine.h>

namespace TH
{
	//Attach this to the UI
	class BabyPointerUI : public BE::IScript
	{
		BE::EntityID cameraObj;		//Reference to the camera object, starting point
		BE::EntityID babyObj;		//Reference to the baby obj, end point

	public:
		void Awake() override;
		void Update(double dt) override;

	};
}

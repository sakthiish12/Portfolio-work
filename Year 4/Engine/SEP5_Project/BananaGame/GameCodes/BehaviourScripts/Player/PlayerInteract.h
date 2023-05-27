#pragma once
#include <BananaEngine.h>
#include "../Character/VirtualCharacterScript.h"

namespace TH
{
	using namespace BE;

	class PlayerInteract : public IScript
	{
		VirtualCharacter* charac_body = nullptr;
		EntityID interactUI;
		float sightDistance = 10.0f;
		float UIAboveOffset = 0.6f;
		bool enabled = true;
		bool InteractThisFrame = false;

		void Awake();
		void Update(double dt);
	public:

		void ToggleInteractEnable(bool in) { enabled = in; }
		inline void TriggerInteractObject() { InteractThisFrame = true; }

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};	
}
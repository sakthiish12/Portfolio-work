#pragma once

#include <BananaEngine.h>
#include "Interactable.h"


namespace TH
{
	using namespace BE;


	class InteractableConsole : public IScript
	{
		std::set<EntityID> tTargetIDs;
		Interactable* interact_base = nullptr;
		std::string Tagtotarget = "DoorUnlock5";
		size_t CutsceneIndex = 3;
		EntityID m_cdoController;

		bool ActivatedOnce = false;
	public:
		void Awake();
		void Init();
		void Update(double dt);
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};
}

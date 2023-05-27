#pragma once
#ifndef	INTERACTABLE_H
#define INTERACTABLE_H

#include <BananaEngine.h>
#include "../Level/LevelManager.h"
namespace TH
{
	using namespace BE;

	class Interactable : public IScript
	{
		
		bool IsInteracted = false;
		bool FirstTimeInteracted = false;

		static LevelManager* iLevelMgr;

		void Awake();
		void Init();
	public:
		bool disableInteraction = false;
		void InteractObject();
		void ResetInteracted();
		bool GetInteracted() const;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};


};
#endif
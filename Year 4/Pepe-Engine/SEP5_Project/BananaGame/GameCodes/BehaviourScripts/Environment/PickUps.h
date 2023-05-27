#pragma once
#ifndef	PICKUPS_H
#define PICKUPS_H

#include <BananaEngine.h>
#include "Interactable.h"
#include "../../UserInterface/InGameUIScripts/MilkBottleUIController.h"


namespace TH
{
	using namespace BE;


	class BananaPickUp : public IScript
	{
		Interactable* interact_base = nullptr;
		PlayerCharacter* player = nullptr;
		uint8_t Bananas_quantity = 32;
		bool HasCalledPlayerAnim = false;

	public:
		void Awake() {}
		void Init();
		void Update(double dt);
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};


	class MilkBottlePickUp : public IScript
	{
		Interactable* interact_base = nullptr;
		MilkBottleUIController* player_milkbottles = nullptr;

	public:
		void Awake() {}
		void Init();
		void Update(double dt);
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};
}

#endif
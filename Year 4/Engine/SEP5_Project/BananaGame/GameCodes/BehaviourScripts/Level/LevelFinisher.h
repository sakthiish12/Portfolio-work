#pragma once
#include <BananaEngine.h>

namespace TH
{
	class LevelFinisher : public BE::IScript
	{
		BE::EntityID gameManager;		//Ref to the game manager entity
		BE::EntityID cdoController;		//Ref to the CDOController
		bool levelFinished = false;

		void Init() override;
		void Update(double dt) override;
		void OnCollision(BE::EntityID collidingID) override;
	};
}
#pragma once
#ifndef	LEVEL_ONE_H
#define LEVEL_ONE_H
#include <BananaEngine.h>
#include "LevelEvents.h"
#include "LevelManager.h"

namespace TH
{
	using namespace BE;

	class LevelOne : /*public LevelEvents,*/ public IScript
	{
		LevelScript* levelscript = nullptr;
	public:
		void Awake();
		void Init();

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

}

#endif
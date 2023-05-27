#pragma once
#ifndef	OFFROBOT_H
#define OFFROBOT_H

#include <BananaEngine.h>

namespace TH
{
	using namespace BE;
	class OffRobot : public IScript
	{
		EntityID model = NULL_ENTITY;
		float timer = 0.0f;
	public:
		void Init();
		void Update(double dt);
	};

}

#endif

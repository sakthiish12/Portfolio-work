#pragma once
#ifndef	GAME_CONFIG_H
#define GAME_CONFIG_H

#include <BananaEngine.h>

namespace BE
{
	constexpr float WORLD_SCALE = 1;
	constexpr Vec3f WORLD_BOUND_MIN = Vec3f(-1000.f);
	constexpr Vec3f WORLD_BOUND_MAX = Vec3f(1000.f);

	inline bool Within_World_Bounds(Vec3f position) {
		return  (position.x >= WORLD_BOUND_MIN.x && position.x <= WORLD_BOUND_MAX.x) &&
				(position.y >= WORLD_BOUND_MIN.y && position.y <= WORLD_BOUND_MAX.y) &&
				(position.z >= WORLD_BOUND_MIN.z && position.z <= WORLD_BOUND_MAX.z);
	}

}

#endif
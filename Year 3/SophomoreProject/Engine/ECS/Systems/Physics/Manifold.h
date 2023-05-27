#ifndef _MANIFOLD_H
#define _MANIFOLD_H

#include "../../../Math/Vector/Vector2.h"

struct Manifold
{
	unsigned int e1;
	unsigned int e2;
	float penetration;
	Mathf::vec2 normal;

	Manifold() :
		e1{ 0 },
		e2{ 0 },
		penetration{ 0.0f },
		normal{ Mathf::vec2() }
	{
	}

	Manifold(unsigned int ent1, unsigned int ent2, float penetration = 0.0f, Mathf::vec2 nrm = Mathf::vec2()) :
		e1{ ent1 },
		e2{ ent2 },
		penetration{ penetration },
		normal{ nrm }
	{
	}
};

#endif

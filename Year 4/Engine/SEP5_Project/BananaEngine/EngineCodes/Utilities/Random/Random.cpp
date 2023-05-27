#include "pch.h"
#include "Random.h"

namespace BE
{
	namespace Rand
	{
		std::mt19937 s_RNGEngine;

		void InitRNG(unsigned int seed)
		{
			s_RNGEngine = std::mt19937(seed);
		}
	}
}
#pragma once
#ifndef Random_H
#define Random_H

#include <Precompiled/pch.h>

namespace BE
{
	namespace Rand
	{
		extern std::mt19937 s_RNGEngine;

		//Must call this function before using any of the RNG functions, leave blank for random seed
		void InitRNG(unsigned int seed = std::random_device()());

		//Any arithmetic types
		template <typename T>
		T Random(T min, T max);
	}
}

namespace PERand = BE::Rand;
#include "Random.hpp"

#endif
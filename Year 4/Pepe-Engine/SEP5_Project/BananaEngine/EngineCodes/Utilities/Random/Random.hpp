#pragma once

namespace BE
{
	namespace Rand
	{
		template <typename T>
		T Random(T min, T max)
		{
			static_assert(std::is_arithmetic<T>::value, "Static Assertion failed: Can only use random number generator on number types");

			//Integer type
			if constexpr (std::is_integral<T>::value)
				return std::uniform_int_distribution<T>(min, max)(s_RNGEngine);
			//Floating type
			else
				return std::uniform_real_distribution<T>(min, max)(s_RNGEngine);
		}
	}
}
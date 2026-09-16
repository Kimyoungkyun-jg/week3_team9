#pragma once

#include <chrono>
#include <random>

// Source: https://www.learncpp.com/cpp-tutorial/global-random-numbers-random-h/

namespace Random
{
	/** Creates a Mersenne Twister initialized with a non-deterministic seed. */
	inline std::mt19937 Generate()
	{
		std::random_device RandomDevice;
		std::seed_seq SeedSequence
		{
			static_cast<std::seed_seq::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()),
			RandomDevice(),
			RandomDevice(),
			RandomDevice(),
			RandomDevice(),
			RandomDevice(),
			RandomDevice(),
			RandomDevice()
		};

		return std::mt19937{ SeedSequence };
	}

	inline std::mt19937 Generator{ Generate() };

	/** Returns a random integer in the inclusive range [InMin, InMax]. */
	inline int Get(int InMin, int InMax)
	{
		return std::uniform_int_distribution{ InMin, InMax }(Generator);
	}

	/** Returns a random integer in the inclusive range [InMin, InMax]. */
	template <typename ValueType>
	ValueType Get(ValueType InMin, ValueType InMax)
	{
		return std::uniform_int_distribution<ValueType>{ InMin, InMax }(Generator);
	}

	/** Returns a random integer after converting both bounds to ReturnType. */
	template <typename ReturnType, typename MinType, typename MaxType>
	ReturnType Get(MinType InMin, MaxType InMax)
	{
		return Get<ReturnType>(static_cast<ReturnType>(InMin), static_cast<ReturnType>(InMax));
	}
}

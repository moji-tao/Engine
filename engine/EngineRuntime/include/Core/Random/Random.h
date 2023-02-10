#pragma once
#include <random>

namespace Engine
{
	class Random
	{
	public:
		static float GetRandom();

		static int GetRandom(int min, int max);

	private:
		static std::default_random_engine engine;
		static std::uniform_real_distribution<float> ds_float;
		static std::uniform_int_distribution<int> ds_int;
	};
}

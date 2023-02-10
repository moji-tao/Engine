#include "EngineRuntime/include/Core/Random/Random.h"
#include "EngineRuntime/include/Core/Base/macro.h"

namespace Engine
{
	std::default_random_engine Random::engine = std::default_random_engine((unsigned int)time(nullptr));
	std::uniform_real_distribution<float> Random::ds_float = std::uniform_real_distribution<float>(0.0f, 1.0f);
	std::uniform_int_distribution<int> Random::ds_int = std::uniform_int_distribution<int>();

	float Random::GetRandom()
	{
		return ds_float(engine);
	}

	int Random::GetRandom(int min, int max)
	{
		ASSERT(min <= max);
		ds_int.param(std::uniform_int_distribution<int>::param_type(min, max));
		return ds_int(engine);
	}
}

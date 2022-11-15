#include "EngineRuntime/include/Core/Random/Random.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace Engine
{
	boost::mt19937 Random::mt = boost::mt19937((uint32_t)time(nullptr));

	float Random::GetRandom()
	{
		static boost::uniform_01<boost::mt19937&>  u01(mt);
		return (float)u01();
	}

	int Random::GetRandom(int min, int max)
	{
		assert(min <= max);
		return min + mt() % (max - min);
	}

	GUID Random::GetGUID()
	{
		boost::uuids::uuid uid = boost::uuids::random_generator()();
		return boost::uuids::to_string(uid);
	}
}

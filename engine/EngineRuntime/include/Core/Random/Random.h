#pragma once
#include <string>
#include <boost/random.hpp>

namespace Engine
{
	using GUID = std::string;

	class Random
	{
	public:
		static float GetRandom();

		static int GetRandom(int min, int max);

		static GUID GetGUID();

	private:
		static boost::mt19937 mt;
	};
}

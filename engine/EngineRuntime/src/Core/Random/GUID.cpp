#include "EngineRuntime/include/Core/Random/GUID.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Core/Random/Random.h"

namespace Engine
{
	GUID::GUID()
	{ }

	GUID::GUID(const std::string& str)
	{
		ASSERT(str.size() == Size());

		mD = str;
	}

	GUID::GUID(const GUID& g)
	{
		//memcpy(mData, g.Data(), Size());
		mD = g.mD;
	}

	const char* GUID::Data() const
	{
		if (mD.size() > 0)
		{
			return mD.c_str();
		}

		return nullptr;
	}

	bool GUID::IsVaild() const
	{
		if (mD.size() == Size())
		{
			return true;
		}

		return false;
	}

	void GUID::SetNoVaild()
	{
		mD.clear();
	}

	GUID GUID::Get()
	{
		GUID result;
		result.mD.resize(Size(), ' ');
		for (auto i = 0; i < Size(); i += 2)
		{
			const auto rc = Random::GetRandom(0, 255);
			sprintf(&result.mD[i], "%02x", rc);
		}

		return result;
	}

	uint16_t GUID::Size()
	{
		return 32;
	}

	bool GUID::operator==(const GUID& otr) const
	{
		//return memcmp(mData, otr.mData, Size()) == 0;
		return mD == otr.mD;
	}
}

#include "EngineRuntime/include/Core/Meta/Object.h"

namespace Engine
{
	Rtti::Rtti(const char* rttiName, const Rtti* baseRtti)
		:mName(rttiName), mBase(baseRtti)
	{ }

	Rtti::~Rtti()
	{ }

	const std::string& Rtti::GetName() const
	{
		return mName;
	}

	bool Rtti::IsSameType(const Rtti& type) const
	{
		return this == &type;
	}

	bool Rtti::CanCastTo(const Rtti& type) const
	{
		const Rtti* ts = this;
		while (!ts->IsSameType(type))
		{
			if(ts->mBase)
			{
				ts = ts->mBase;
			}
			else
			{
				return false;
			}
		}
		return true;
	}

	const Rtti* Rtti::GetBase() const
	{
		return mBase;
	}

	const Rtti& Object::GetType() const
	{
		return mType;
	}

	const Rtti Object::mType("Object", nullptr);

	void Object::SetGuid(const GUID& guid)
	{
		mGuid = guid;
	}

	GUID Object::GetGuid() const
	{
		return mGuid;
	}


}

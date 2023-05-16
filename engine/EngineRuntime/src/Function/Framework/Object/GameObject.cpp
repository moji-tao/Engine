#include "EngineRuntime/include/Function/Framework/Object/GameObject.h"
#include "EngineRuntime/include/Core/Meta/Reflection.h"

namespace Engine
{
	IMPLEMENT_RTTI(GameObject, Object);

	void GameObject::SetEnable(bool value)
	{
		if (value != mIsEnable)
		{
			mIsEnable = value;

			if (mIsEnable)
			{
				OnEnable();
			}
		}
	}

	bool GameObject::GetEnable()
	{
		return mIsEnable;
	}

	void GameObject::SetLuaTableInstance(LuaTable table)
	{
		mLuaGC = true;
		mLuaTable = table;
	}

}

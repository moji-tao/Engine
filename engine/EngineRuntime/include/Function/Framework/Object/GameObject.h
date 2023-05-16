#pragma once
#include "EngineRuntime/include/Core/Meta/Object.h"
#include "EngineRuntime/include/Framework/Interface/ISerializable.h"
#include "EngineRuntime/include/Function/Framework/ScriptSystem.h"

namespace Engine
{
	class GameObject : public Object, public ISerializable
	{
		DECLARE_RTTI;
	public:
		GameObject() = default;

		virtual ~GameObject() = default;

	public:
		virtual void CloneData(GameObject* node) = 0;

	public:
		virtual void Awake() = 0;

		virtual void Tick(float deltaTime) = 0;

		virtual void OnDestroy() = 0;

		virtual void OnEnable() = 0;

		void SetEnable(bool value);

		bool GetEnable();

	public:
		// Lua new
		void SetLuaTableInstance(LuaTable table);

	protected:
		LuaTable mLuaTable;

		bool mLuaGC = false;

		bool mIsEnable = true;
	};
}

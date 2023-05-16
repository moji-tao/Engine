#pragma once
#include <sol/sol.hpp>
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"

namespace Engine
{
	using LuaTable = sol::table;

	class ScriptSystem : public ISingleton<ScriptSystem>
	{
	public:
		ScriptSystem();

		virtual ~ScriptSystem();

	public:
		virtual bool Initialize();

		virtual bool Tick(float deltaTile, bool isEditorMode);

		virtual void Finalize();

	private:
		void InitLuaGlobal();

		void InitLuaPath();

	private:
		sol::state mLuaState;
	};
}

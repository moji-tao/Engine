#pragma once
#include <chrono>
#include <string>
#include "EngineRuntime/include/Framework/Interface/IRuntimeModule.h"
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"

namespace Engine
{
	class EngineRuntime : public ISingleton<EngineRuntime>
	{
	public:
		EngineRuntime() = default;

		virtual ~EngineRuntime();

	public:
		virtual bool Initialize(InitConfig* info);

		virtual void Finalize();

		virtual bool Tick(float deltaTime, bool isEditorMode);

		float GetDeltaTime();

	private:
		std::chrono::steady_clock::time_point mLastTickTimePoint;

	private:
		float mEachFrameTime = 0;
	};
}

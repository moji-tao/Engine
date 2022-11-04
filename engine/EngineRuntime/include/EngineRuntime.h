#pragma once
#include <chrono>
#include <string>
#include "EngineRuntime/include/Framework/Interface/IRuntimeModule.h"
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"

namespace Engine
{
	class EngineRuntime : public IRuntimeModule, public ISingleton<EngineRuntime>
	{
	public:
		EngineRuntime() = default;

		virtual ~EngineRuntime() override;

	public:
		virtual bool Initialize(InitConfig* info) override;

		//void Run();

		virtual void Finalize() override;

		virtual bool Tick(float deltaTime) override;

		float GetDeltaTime();

	private:
		std::chrono::steady_clock::time_point mLastTickTimePoint;
	};
}

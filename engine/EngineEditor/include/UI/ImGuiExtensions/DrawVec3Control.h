#pragma once
#include <string>
#include "EngineRuntime/include/Core/Math/Vector3.h"

namespace ImGui
{
	void DrawVec3Control(const std::string& label, Engine::Vector3& values);

}

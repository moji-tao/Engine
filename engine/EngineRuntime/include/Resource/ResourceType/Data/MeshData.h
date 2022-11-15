#pragma once
#include <vector>
#include "EngineRuntime/include/Core/Math/Vector3.h"
#include "EngineRuntime/include/Core/Math/Vector2.h"

namespace Engine
{
	struct Vertex
	{
		// 位置
		Vector3 Position;
		// 法向量
		Vector3 Normal;
		// 切线
		Vector3 Tangent;
		// 副切线
		Vector3 Bitangent;
		// 纹理坐标
		Vector2 TexCoords;
	};

	struct Mesh
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};
}

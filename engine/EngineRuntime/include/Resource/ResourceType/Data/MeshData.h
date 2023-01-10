#pragma once
#include <vector>
#include "EngineRuntime/include/Core/Math/Vector3.h"
#include "EngineRuntime/include/Core/Math/Vector2.h"
#include "EngineRuntime/include/Core/Math/Vector4.h"
#include "EngineRuntime/include/Framework/Interface/ISerializable.h"

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

	class SubMesh : public ISerializable
	{
	public:
		virtual void Serialize(Engine::SerializerDataFrame& stream) const override;

		virtual bool Deserialize(Engine::SerializerDataFrame& stream) override;

	public:
		std::vector<Vertex> Vertices;
		std::vector<unsigned int> Indices;
		uint32_t MaterialIndex;
	};

	struct Mesh : public ISerializable
	{
	public:
		virtual void Serialize(Engine::SerializerDataFrame& stream) const override;

		virtual bool Deserialize(Engine::SerializerDataFrame& stream) override;

	public:
		std::vector<SubMesh> Meshes;
	};
}

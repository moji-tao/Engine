#include "EngineRuntime/include/Resource/ResourceType/Data/MeshData.h"

namespace Engine
{
	void SubMesh::Serialize(SerializerDataFrame& stream) const
	{
		stream << Vertices;
		stream << Indices;
		stream << MaterialIndex;
	}

	bool SubMesh::Deserialize(SerializerDataFrame& stream)
	{
		stream >> Vertices;
		stream >> Indices;
		stream >> MaterialIndex;

		return true;
	}

	void Mesh::Serialize(Engine::SerializerDataFrame& stream) const
	{
		stream << Meshes;
	}

	bool Mesh::Deserialize(Engine::SerializerDataFrame& stream)
	{
		stream >> Meshes;

		return true;
	}

}


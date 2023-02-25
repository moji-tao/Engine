#pragma once
#include <vector>
#include "EngineRuntime/include/Core/Math/Vector3.h"
#include "EngineRuntime/include/Core/Math/Vector2.h"
#include "EngineRuntime/include/Core/Math/Collision.h"
#include "EngineRuntime/include/Core/Meta/Object.h"
#include "EngineRuntime/include/Framework/Interface/ISerializable.h"

namespace Engine
{
	struct Vertex
	{
		Vertex() = default;

		Vertex(const Vector3& position, const Vector3& normal, const Vector3& tangent, const Vector2& texCoords);

		Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty, float tz, float u, float v);

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
		// 骨骼索引
		uint32_t BoneIndices[4];
		// 骨骼权重
		Vector4 BoneWeights;
		// 骨骼数
		uint32_t BoneNum;
	};

	class SubMesh : public ISerializable
	{
	public:
		SubMesh() = default;

		virtual ~SubMesh() override = default;

	public:
		virtual void Serialize(Engine::SerializerDataFrame& stream) const override;

		virtual bool Deserialize(Engine::SerializerDataFrame& stream) override;

	private:
		void Subdivide();

		Vertex MidPoint(const Vertex& v0, const Vertex& v1);

		void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount);

		void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount);

		friend class Mesh;
	public:
		std::vector<Vertex> Vertices;
		std::vector<unsigned int> Indices;
		BoundingBox mBoundingBox;
		uint32_t MaterialIndex;
		GUID mGuid;
	};

	class Mesh : public Object, public ISerializable
	{
		DECLARE_RTTI;
	public:
		using uint32 = uint32_t;

		Mesh() = default;

		virtual ~Mesh() override = default;

	public:
		virtual void Serialize(Engine::SerializerDataFrame& stream) const override;

		virtual bool Deserialize(Engine::SerializerDataFrame& stream) override;

	public:
		static void CreateBox(Mesh& mesh, float width, float height, float depth, uint32_t numSubdivisions);

		static void CreateSphere(Mesh& mesh, float radius, uint32_t sliceCount, uint32_t stackCount);

		static void CreateCylinder(Mesh& mesh, float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount);

		static void CreateGrid(Mesh& mesh, float width, float depth, uint32_t m, uint32_t n);

		static void CreateQuad(Mesh& mesh, float x, float y, float w, float h, float depth);

	public:
		std::vector<SubMesh> Meshes;
	};

	class MeshMeta : public MetaFrame
	{
	public:
		MeshMeta() = default;
		virtual ~MeshMeta() override = default;

	public:
		void Load(const std::filesystem::path& metaPath);

		virtual void Save(const std::filesystem::path& metaPath) override;
	};
}

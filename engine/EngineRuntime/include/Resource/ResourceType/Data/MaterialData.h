#pragma once
#include "EngineRuntime/include/Core/Meta/Object.h"
#include "EngineRuntime/include/Core/Math/Vector4.h"
#include "EngineRuntime/include/Core/Math/Color.h"
#include "EngineRuntime/include/Framework/Interface/ISerializable.h"

namespace Engine
{
	class MaterialData : public Object, public ISerializable
	{
		DECLARE_RTTI;
	public:
		void Serialize(SerializerDataFrame& stream) const override;

		bool Deserialize(SerializerDataFrame& stream) override;

	public:
		// 反照率
		Vector4 Albedo = Colors::WhiteSmoke;
		GUID RefAlbedoTexture;

		// 法线
		GUID RefNormalTexture;

		// 金属度
		float Metallic = 0.0f;
		GUID RefMetallicTexture;

		// 粗糙度
		float Roughness = 0.0f;
		GUID RefRoughnessTexture;

		// 自发光
		Vector3 Emissive = Vector3::ZERO;
	};

	class MaterialMeta : public MetaFrame
	{
	public:
		MaterialMeta() = default;

		virtual ~MaterialMeta() override = default;

	public:
		void Load(const std::filesystem::path& metaPath);

		virtual void Save(const std::filesystem::path& metaPath) override;
	};
}

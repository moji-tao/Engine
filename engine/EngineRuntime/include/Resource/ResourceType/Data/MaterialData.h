#pragma once
#include "EngineRuntime/include/Core/Meta/Object.h"
#include "EngineRuntime/include/Core/Math/Vector3.h"
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
		// 漫反射颜色
		Vector3 DiffuseColor;
		// 高光颜色
		Vector3 SpecularColor;
		// 环境颜色
		Vector3 AmbientColor;
		// 反射颜色
		Vector3 ReflectiveColor;
		// 自发光颜色
		Vector3 EmissiveColor;
		// 透明色
		Vector3 TransparentColor;
		// 不透明度
		float Opacity;
		// 粗糙度
		float Shininess;
		// 漫反射纹理路径
		GUID RefDiffuseTexture;
		// 高光纹理路径
		GUID RefSpecularTexture;
		// 法线贴图路径
		GUID RefNormalTexture;
		// 高度贴图路径
		GUID RefHeightTexture;
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

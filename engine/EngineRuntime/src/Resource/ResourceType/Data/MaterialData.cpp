#include <fstream>
#include <yaml-cpp/yaml.h>
#include "EngineRuntime/include/Resource/ResourceType/Data/MaterialData.h"

namespace Engine
{
	IMPLEMENT_RTTI(MaterialData, Object);

	void MaterialData::Serialize(SerializerDataFrame& stream) const
	{
		stream << Albedo;
		stream << RefAlbedoTexture;
		stream << RefNormalTexture;
		stream << Metallic;
		stream << RefMetallicTexture;
		stream << Roughness;
		stream << RefRoughnessTexture;
		stream << Emissive;
	}

	bool MaterialData::Deserialize(SerializerDataFrame& stream)
	{
		stream >> Albedo;
		stream >> RefAlbedoTexture;
		stream >> RefNormalTexture;
		stream >> Metallic;
		stream >> RefMetallicTexture;
		stream >> Roughness;
		stream >> RefRoughnessTexture;
		stream >> Emissive;
		return true;
	}

	void MaterialMeta::Load(const std::filesystem::path& metaPath)
	{
		YAML::Node node = YAML::LoadFile(ProjectFileSystem::GetInstance()->GetActualPath(metaPath).generic_string());

		ASSERT(node["guid"].IsDefined());

		if (mGuid != nullptr)
		{
			delete mGuid;
		}

		mGuid = new GUID(node["guid"].as<std::string>());
	}

	void MaterialMeta::Save(const std::filesystem::path& metaPath)
	{
		ASSERT(mGuid != nullptr);

		YAML::Node node;
		node["guid"] = mGuid->Data();

		std::ofstream fout(ProjectFileSystem::GetInstance()->GetActualPath(metaPath));
		fout << node;
	}
}


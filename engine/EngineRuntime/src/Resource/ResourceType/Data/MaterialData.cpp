#include <fstream>
#include <yaml-cpp/yaml.h>
#include "EngineRuntime/include/Resource/ResourceType/Data/MaterialData.h"

namespace Engine
{
	IMPLEMENT_RTTI(MaterialData, Object);

	void MaterialData::Serialize(SerializerDataFrame& stream) const
	{
		stream << DiffuseColor;
		stream << SpecularColor;
		stream << AmbientColor;
		stream << ReflectiveColor;
		stream << EmissiveColor;
		stream << TransparentColor;
		stream << Opacity;
		stream << Shininess;
		stream << RefDiffuseTexture;
		stream << RefSpecularTexture;
	}

	bool MaterialData::Deserialize(SerializerDataFrame& stream)
	{
		stream >> DiffuseColor;
		stream >> SpecularColor;
		stream >> AmbientColor;
		stream >> ReflectiveColor;
		stream >> EmissiveColor;
		stream >> TransparentColor;
		stream >> Opacity;
		stream >> Shininess;
		stream >> RefDiffuseTexture;
		stream >> RefSpecularTexture;

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


#include <fstream>
#include <yaml-cpp/yaml.h>
#include "EngineRuntime/include/Resource/ResourceType/Data/SkeletonData.h"

namespace Engine
{
	IMPLEMENT_RTTI(SkeletonData, Object);

	void SkeletonData::Serialize(Engine::SerializerDataFrame& stream) const
	{
	}

	bool SkeletonData::Deserialize(Engine::SerializerDataFrame& stream)
	{
		return false;
	}

	void SkeletonMeta::Load(const std::filesystem::path& metaPath)
	{
		YAML::Node node = YAML::LoadFile(ProjectFileSystem::GetInstance()->GetActualPath(metaPath).generic_string());

		ASSERT(node["guid"].IsDefined());

		if (mGuid != nullptr)
		{
			delete mGuid;
		}

		mGuid = new GUID(node["guid"].as<std::string>());
	}

	void SkeletonMeta::Save(const std::filesystem::path& metaPath)
	{
		ASSERT(mGuid != nullptr);

		YAML::Node node;
		node["guid"] = mGuid->Data();

		std::ofstream fout(ProjectFileSystem::GetInstance()->GetActualPath(metaPath));
		fout << node;
	}
}

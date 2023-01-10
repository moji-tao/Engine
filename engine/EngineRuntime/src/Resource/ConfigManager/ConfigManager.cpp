#include <fstream>
#include <json/json.h>
#include "EngineRuntime/include/Resource/ConfigManager/ConfigManager.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"

namespace Engine
{
	bool ConfigManager::Initialize(const std::filesystem::path& enginePath)
	{
		LoadAssertConfig();
		return true;
	}

	void ConfigManager::Finalize()
	{
		SaveAssertConfig();
	}

	std::filesystem::path ConfigManager::GetDefaultScenePath()
	{
		return mCurrentDefaultScenePath;
	}

	void ConfigManager::SetDefaultScenePath(const std::filesystem::path& defaultPath)
	{
		mCurrentDefaultScenePath = defaultPath;
	}

	void ConfigManager::LoadAssertConfig()
	{
		std::shared_ptr<Blob> blob = ProjectFileSystem::GetInstance()->ReadFile("Library/ProjectConfig.json");

		Json::Reader reader;
		Json::Value root;

		if (reader.parse((char*)blob->GetData(), (char*)blob->GetData() + blob->GetSize(), root))
		{
			mCurrentDefaultScenePath = root["DefaultScene"].asString();
		}

	}

	void ConfigManager::SaveAssertConfig()
	{
		Json::Value root;
		root["DefaultScene"] = mCurrentDefaultScenePath.generic_string();

		Json::Value def = []() {
			Json::Value def;
			Json::StreamWriterBuilder::setDefaults(&def);
			def["emitUTF8"] = true;
			return def;
		}();

		std::ofstream stream(ProjectFileSystem::GetInstance()->GetActualPath("Library/ProjectConfig.json"));
		Json::StreamWriterBuilder stream_builder;
		stream_builder.settings_ = def;//Config emitUTF8
		std::unique_ptr<Json::StreamWriter> writer(stream_builder.newStreamWriter());
		writer->write(root, &stream);
	}

}

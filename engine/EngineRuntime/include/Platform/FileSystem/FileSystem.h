#pragma once
#include <filesystem>
#include "EngineRuntime/include/Framework/Interface/IRuntimeModule.h"
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"

namespace Engine
{
	using namespace std::filesystem;

	class FileSystem : public IRuntimeModule, public ISingleton<FileSystem>
	{
	public:
		FileSystem() = default;

		virtual ~FileSystem() override;

	public:
		enum FileSystemType
		{
			EnginePath = 0u,
			AssectPath,

			End
		};

	public:
		virtual bool Initialize(InitConfig* info) override;

		virtual bool Tick(float deltaTile) override;

		virtual void Finalize() override;

	public:
		bool CreateDirectory(FileSystemType type, const path& filePath);

		path GetActualPath(FileSystemType type, const path& filePath);

		bool Exists(FileSystemType type, const path& filePath);

		uintmax_t GetFileSize(FileSystemType type, const path& filePath);

	private:
		path mRootPath[End];
	};
}

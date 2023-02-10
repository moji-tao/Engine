#pragma once
#include "EngineRuntime/include/Framework/Interface/ISerializable.h"

namespace Engine
{
	class TextMeta : public MetaFrame
	{
	public:
		TextMeta() = default;
		virtual ~TextMeta() override = default;

	public:
		void Load(const std::filesystem::path& metaPath);

		virtual void Save(const std::filesystem::path& metaPath) override;
	};

}

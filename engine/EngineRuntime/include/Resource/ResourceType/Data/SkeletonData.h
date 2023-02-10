#pragma once
#include "EngineRuntime/include/Core/Meta/Object.h"
#include "EngineRuntime/include/Framework/Interface/ISerializable.h"

namespace Engine
{
	struct SkeletonData : public Object, public ISerializable
	{
		DECLARE_RTTI;
	public:
		virtual void Serialize(Engine::SerializerDataFrame& stream) const override;

		virtual bool Deserialize(Engine::SerializerDataFrame& stream) override;

	};

	class SkeletonMeta : public MetaFrame
	{
	public:
		SkeletonMeta() = default;
		virtual ~SkeletonMeta() override = default;

	public:
		void Load(const std::filesystem::path& metaPath);

		virtual void Save(const std::filesystem::path& metaPath) override;
	};

}

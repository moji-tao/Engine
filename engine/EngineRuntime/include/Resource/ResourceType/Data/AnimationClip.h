#pragma once
#include "EngineRuntime/include/Core/Meta/Object.h"
#include "EngineRuntime/include/Framework/Interface/ISerializable.h"

namespace Engine
{
	class AnimationClip : public Object, public ISerializable
	{
		DECLARE_RTTI;
	public:
		AnimationClip();

		virtual ~AnimationClip() override;

	public:
		bool GetBoneKeyMat(const std::string& boneName, double keyFrame, Matrix4x4& mat);

		void AddBoneKey(const std::string& boneName, double keyTime, const Transform& transform);

		void SetAnimationTime(double time);

		void SetTicksPerSecond(double tc);

		double GetAnimationTime();

		double GetTicksPerSecond();

		std::string GetAnimationClipName();

		void SetAnimationClipName(const std::string& name);

	public:
		std::string mName;

		double mAnimationLength;

		double mTicksPerSecond;

		std::unordered_map<std::string, std::map<double, Transform>> mAnimation;

	private:
		void GetKeyFrameLerp(double keyFrameTime, const std::map<double, Transform>& keyFrameMap, Matrix4x4& mat);

	public:
		virtual void Serialize(SerializerDataFrame& stream) const override;

		virtual bool Deserialize(SerializerDataFrame& stream) override;
	};

	class AnimationClipMeta : public MetaFrame
	{
	public:
		AnimationClipMeta() = default;

		virtual ~AnimationClipMeta() override = default;

	public:
		void Load(const std::filesystem::path& metaPath);

		virtual void Save(const std::filesystem::path& metaPath) override;
	};

}

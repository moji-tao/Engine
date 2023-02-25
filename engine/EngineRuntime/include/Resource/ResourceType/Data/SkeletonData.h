#pragma once
#include "EngineRuntime/include/Core/Meta/Object.h"
#include "EngineRuntime/include/Core/Math/Transform.h"
#include "EngineRuntime/include/Framework/Interface/ISerializable.h"

namespace Engine
{
	class Joint : public Object, public ISerializable
	{
		DECLARE_RTTI;
	public:
		Joint();

		virtual ~Joint() override;

	public:
		std::string mJointName;
		Transform mTransform;
		Matrix4x4 mModleToLocalMat;
		Joint* mParent;
		uint32_t mJointNum;
		std::vector<Joint*> mChildrens;

	public:
		virtual void Serialize(SerializerDataFrame& stream) const override;

		virtual bool Deserialize(SerializerDataFrame& stream) override;
	};

	struct SkeletonData : public Object, public ISerializable
	{
		DECLARE_RTTI;
	public:
		void AddRootJoint(Joint* rootJoint);

		void AddJoint(const std::string& jointName, Joint* joint);

		Joint* GetJoint(const std::string& name) const;

		Joint* GetRootJoint() const;

		uint32_t GetJointCount() const;

	private:
		Joint* mRootJoint = nullptr;

		std::unordered_map<std::string, Joint*> mJointMap;

		uint32_t mJointCount;

	public:
		virtual void Serialize(SerializerDataFrame& stream) const override;

		virtual bool Deserialize(SerializerDataFrame& stream) override;

	private:
		void LoadJointMap(Joint* joint);
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

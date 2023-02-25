#include <fstream>
#include <yaml-cpp/yaml.h>
#include "EngineRuntime/include/Resource/ResourceType/Data/SkeletonData.h"

namespace Engine
{
	IMPLEMENT_RTTI(Joint, Object);
	IMPLEMENT_RTTI(SkeletonData, Object);

	Joint::Joint()
		:mParent(nullptr)
	{ }

	Joint::~Joint()
	{
		for (int i = 0; i < mChildrens.size(); ++i)
		{
			if (mChildrens[i] != nullptr)
			{
				delete mChildrens[i];
				mChildrens[i] = nullptr;
			}
		}
	}

	void Joint::Serialize(SerializerDataFrame& stream) const
	{
		stream << mJointName;
		stream << mTransform;
		stream << mJointNum;
		stream << mModleToLocalMat;
		stream << (uint32_t)mChildrens.size();

		for (int i = 0; i < mChildrens.size(); ++i)
		{
			stream << *mChildrens[i];
		}
	}

	bool Joint::Deserialize(SerializerDataFrame& stream)
	{
		stream >> mJointName;
		stream >> mTransform;
		stream >> mJointNum;
		stream >> mModleToLocalMat;
		uint32_t mChildNum;
		stream >> mChildNum;
		mChildrens.resize(mChildNum);

		for (int i = 0; i < mChildNum; ++i)
		{
			mChildrens[i] = new Joint;
			stream >> *mChildrens[i];
			mChildrens[i]->mParent = this;
		}

		return true;
	}

	void SkeletonData::AddRootJoint(Joint* rootJoint)
	{
		mRootJoint = rootJoint;
	}

	void SkeletonData::AddJoint(const std::string& jointName, Joint* joint)
	{
		mJointMap[jointName] = joint;
	}

	Joint* SkeletonData::GetJoint(const std::string& name) const
	{
		auto it = mJointMap.find(name);
		if (it == mJointMap.end())
		{
			return nullptr;
		}

		return it->second;
	}

	Joint* SkeletonData::GetRootJoint() const
	{
		return mRootJoint;
	}

	uint32_t SkeletonData::GetJointCount() const
	{
		return mJointCount;
	}

	void SkeletonData::Serialize(SerializerDataFrame& stream) const
	{
		stream << *mRootJoint;
	}

	bool SkeletonData::Deserialize(SerializerDataFrame& stream)
	{
		if (mRootJoint != nullptr)
		{
			delete mRootJoint;
		}
		mRootJoint = new Joint;
		stream >> *mRootJoint;

		LoadJointMap(mRootJoint);

		mJointCount = mJointMap.size();

		return true;
	}

	void SkeletonData::LoadJointMap(Joint* joint)
	{
		mJointMap[joint->mJointName] = joint;

		for (int i = 0; i < joint->mChildrens.size(); ++i)
		{
			LoadJointMap(joint->mChildrens[i]);
		}
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

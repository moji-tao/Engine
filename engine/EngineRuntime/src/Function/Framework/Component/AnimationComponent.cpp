#include "EngineRuntime/include/Function/Framework/Component/AnimationComponent.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/AnimationClip.h"

namespace Engine
{
	AnimationComponent::AnimationComponent()
		:Component(), mCurrentTickAnimationTime(0.0)
	{
		mOrder = 2;
	}

	AnimationComponent::~AnimationComponent()
	{ }

	void AnimationComponent::Tick(float deltaTime)
	{
		if (!mIsEnable)
		{
			return;
		}

		if (!mRefAnimationRes.IsVaild())
		{
			return;
		}

		AnimationClip* animationClip = AssetManager::GetInstance()->LoadResource<AnimationClip>(mRefAnimationRes);

		if (animationClip == nullptr)
		{
			return;
		}

		mCurrentTickAnimationTime += deltaTime;

		double animationLength = animationClip->GetAnimationTime();

		if (mCurrentTickAnimationTime >= animationLength)
		{
			mCurrentTickAnimationTime -= animationLength;
		}
	}

	bool AnimationComponent::GetCurrentTickBoneOffsetMat(const std::string& boneName, Matrix4x4& mat)
	{
		if (!mRefAnimationRes.IsVaild())
		{
			mat = Matrix4x4::IDENTITY;
			return false;
		}

		AnimationClip* animationClip = AssetManager::GetInstance()->LoadResource<AnimationClip>(mRefAnimationRes);

		if (animationClip == nullptr)
		{
			mat = Matrix4x4::IDENTITY;
			return false;
		}

		if (!animationClip->GetBoneKeyMat(boneName, mCurrentTickAnimationTime, mat))
		{
			return false;
		}

		return true;
	}

	bool AnimationComponent::GetCurrentTickBoneOffsetTransform(const std::string& boneName, Transform& transform)
	{
		if (!mRefAnimationRes.IsVaild())
		{
			return false;
		}

		AnimationClip* animationClip = AssetManager::GetInstance()->LoadResource<AnimationClip>(mRefAnimationRes);

		if (animationClip == nullptr)
		{
			return false;
		}

		if (!animationClip->GetBoneKeyTransform(boneName, mCurrentTickAnimationTime, transform))
		{
			return false;
		}

		return true;
	}

	GUID AnimationComponent::GetAnimationGuid()
	{
		return mRefAnimationRes;
	}

	void AnimationComponent::SetAnimationGuid(const GUID& guid)
	{
		mRefAnimationRes = guid;
	}

	void AnimationComponent::Serialize(SerializerDataFrame& stream) const
	{
		stream << mRefAnimationRes;
	}

	bool AnimationComponent::Deserialize(SerializerDataFrame& stream)
	{
		stream >> mRefAnimationRes;

		mCurrentTickAnimationTime = 0.0;

		return true;
	}

	void AnimationComponent::CloneData(GameObject* node)
	{
		AnimationComponent* bluePrint = DynamicCast<AnimationComponent>(node);

		mRefAnimationRes = bluePrint->mRefAnimationRes;
	}
}

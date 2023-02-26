#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/AnimationClip.h"
#include "EngineRuntime/include/Function/Framework/Component/AnimatorComponent.h"

#include "EngineRuntime/include/Core/Math/Math.h"

namespace Engine
{
	AnimatorComponent::AnimatorComponent()
		:Component()
	{
		mOrder = 2;
	}

	AnimatorComponent::~AnimatorComponent()
	{ }

	void AnimatorComponent::Tick(float deltaTime)
	{
		size_t mAnimCount = mAnimationBlending.size();
		mCurrentTickAnimationTime += deltaTime;
		if (mAnimCount == 0)
		{
			mFirstAnimationClipRes.SetNoVaild();
			mSecondAnimationClipRes.SetNoVaild();
			return;
		}
		else
		{
			if (mWeight < mAnimationBlending.front().second)
			{
				mWeight = mAnimationBlending.front().second;
			}
			else if (mWeight > mAnimationBlending.back().second)
			{
				mWeight = mAnimationBlending.back().second;
			}

			for (size_t i = 1; i < mAnimCount; ++i)
			{
				if (mWeight >= mAnimationBlending[i - 1].second && mWeight <= mAnimationBlending[i].second)
				{
					mFirstAnimationClipRes = mAnimationBlending[i - 1].first;
					mFirstWeight = mAnimationBlending[i - 1].second;
					mSecondAnimationClipRes = mAnimationBlending[i].first;
					mSecondWeight = mAnimationBlending[i].second;

					mSecondWeight = (mWeight - mAnimationBlending[i - 1].second) / (mAnimationBlending[i].second - mAnimationBlending[i - 1].second);
					mFirstWeight = 1.0f - mSecondWeight;
					return;
				}
			}
			mFirstAnimationClipRes = mSecondAnimationClipRes = mAnimationBlending.back().first;
			mFirstWeight = 0.0f;
			mSecondWeight = 1.0f;
		}
	}

	std::vector<std::pair<GUID, float>>& AnimatorComponent::GetAnimationBlending()
	{
		return mAnimationBlending;
	}

	void AnimatorComponent::SetAnimation(uint32_t index, const GUID& animationGuid)
	{
		mAnimationBlending[index].first = animationGuid;
	}

	void AnimatorComponent::SetAnimationWeight(uint32_t index, float weight)
	{
		mAnimationBlending[index].second = weight;

		SortAnimation();
	}

	void AnimatorComponent::AddAnimation()
	{
		mAnimationBlending.emplace_back();
		if (mAnimationBlending.size() > 1)
		{
			mAnimationBlending.back().second = mAnimationBlending[mAnimationBlending.size() - 2].second + 1;
		}
	}

	void AnimatorComponent::RemoveAnimation()
	{
		if (mAnimationBlending.size() > 0)
		{
			mAnimationBlending.pop_back();
		}
		if (mAnimationBlending.size() == 0)
		{
			mWeight = 0;
		}
		else
		{
			mWeight = Math::Min(mWeight, mAnimationBlending.back().second);
		}
	}

	void AnimatorComponent::SortAnimation()
	{
		std::sort(mAnimationBlending.begin(), mAnimationBlending.end(),
			[](const std::pair<GUID, float>& p1, const std::pair<GUID, float>& p2)
			{
				return p1.second < p2.second;
			});
		mWeight = Math::Min(mWeight, mAnimationBlending.back().second);
	}

	bool AnimatorComponent::GetCurrentTickBoneOffsetMat(const std::string& boneName, Matrix4x4& mat)
	{
		Transform transform;
		if (GetCurrentTickBoneOffsetTransform(boneName, transform))
		{
			mat = transform.getMatrix();
			return true;
		}

		return false;
	}

	bool AnimatorComponent::GetCurrentTickBoneOffsetTransform(const std::string& boneName, Transform& transform)
	{
		if (!mFirstAnimationClipRes.IsVaild() || !mSecondAnimationClipRes.IsVaild())
		{
			return false;
		}

		AnimationClip* firstAnimation = AssetManager::GetInstance()->LoadResource<AnimationClip>(mFirstAnimationClipRes);
		AnimationClip* secondAnimation = AssetManager::GetInstance()->LoadResource<AnimationClip>(mSecondAnimationClipRes);

		if (firstAnimation == nullptr || secondAnimation == nullptr)
		{
			return false;
		}
		double firstAnimationLength = firstAnimation->GetAnimationTime();
		double secondAnimationLength = secondAnimation->GetAnimationTime();
		double weightAnimationLength = firstAnimationLength * mFirstWeight + secondAnimationLength * mSecondWeight;
		while (mCurrentTickAnimationTime >= weightAnimationLength)
		{
			mCurrentTickAnimationTime -= weightAnimationLength;
		}
		double alpha = mCurrentTickAnimationTime / weightAnimationLength;

		Transform firstTrans;
		Transform secondTrans;
		bool firstB = firstAnimation->GetBoneKeyTransform(boneName, firstAnimationLength * alpha, firstTrans);
		bool secondB = secondAnimation->GetBoneKeyTransform(boneName, secondAnimationLength * alpha, secondTrans);

		if (!firstB && !secondB)
		{
			return false;
		}

		if (firstB && !secondB)
		{
			transform = firstTrans;
		}
		else if (!firstB && secondB)
		{
			transform = secondTrans;
		}
		else
		{
			transform.position = Vector3::Lerp(firstTrans.position, secondTrans.position, mSecondWeight);
			transform.scale = Vector3::Lerp(firstTrans.scale, secondTrans.scale, mSecondWeight);
			transform.rotation = Quaternion::nLerp(mSecondWeight, firstTrans.rotation, secondTrans.rotation, true);
		}
		return true;
	}

	void AnimatorComponent::Serialize(SerializerDataFrame& stream) const
	{
		stream << mAnimationBlending;
		stream << mWeight;
	}

	bool AnimatorComponent::Deserialize(SerializerDataFrame& stream)
	{
		stream >> mAnimationBlending;
		stream >> mWeight;

		return true;
	}

	void AnimatorComponent::CloneData(GameObject* node)
	{
		AnimatorComponent* bluePrint = DynamicCast<AnimatorComponent>(node);

		mAnimationBlending = bluePrint->mAnimationBlending;
		mWeight = bluePrint->mWeight;
	}
}

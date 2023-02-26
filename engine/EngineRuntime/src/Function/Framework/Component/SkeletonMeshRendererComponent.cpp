#include "EngineRuntime/include/Function/Framework/Component/SkeletonMeshRendererComponent.h"

#include <queue>

#include "EngineRuntime/include/Function/Framework/Component/AnimationComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/AnimatorComponent.h"
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"
#include "EngineRuntime/include/Function/Render/RenderResource.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"

namespace Engine
{
	SkeletonMeshRendererComponent::SkeletonMeshRendererComponent()
		:Component()
	{
		mOrder = 100;
	}

	SkeletonMeshRendererComponent::~SkeletonMeshRendererComponent()
	{ }

	void SkeletonMeshRendererComponent::Tick(float deltaTime)
	{
		auto transform_component = mParentObject->GetComponent<TransformComponent>();

		RenderResource* resource = RenderSystem::GetInstance()->GetRenderResource();

		ASSERT(resource != nullptr && transform_component != nullptr);

		ObjectConstants constans;
		transform_component->GetRenderMatrix(constans.World, constans.PreWorld);

		constans.InvWorld = constans.World.Inverse();

		do
		{
			if (mRefSkeleton.IsVaild())
			{
				if (!mRefMesh.IsVaild())
				{
					return;
				}
				SkeletonData* skeleton = AssetManager::GetInstance()->LoadResource<SkeletonData>(mRefSkeleton);
				if (skeleton == nullptr)
				{
					break;
				}
				Joint* joint = skeleton->GetRootJoint();
				if (joint == nullptr)
				{
					break;
				}
				std::vector<Matrix4x4> vec;
				std::vector<Matrix4x4> localToModel;
				AnimationComponent* animationComponent = mParentObject->GetComponent<AnimationComponent>();
				AnimatorComponent* animatorComponent = mParentObject->GetComponent<AnimatorComponent>();

				vec.resize(skeleton->GetJointCount());
				localToModel.resize(skeleton->GetJointCount());

				Matrix4x4 offsetMat = Matrix4x4::IDENTITY;

				if (animatorComponent != nullptr)
				{
					if (!animatorComponent->GetCurrentTickBoneOffsetMat(joint->mJointName, offsetMat))
					{
						offsetMat = joint->mTransform.getMatrix();
					}
				}
				else if (animationComponent != nullptr)
				{
					if (!animationComponent->GetCurrentTickBoneOffsetMat(joint->mJointName, offsetMat))
					{
						offsetMat = joint->mTransform.getMatrix();
					}
				}
				else
				{
					offsetMat = joint->mTransform.getMatrix();
				}

				vec[joint->mJointNum] = joint->mModleToLocalMat * offsetMat;
				localToModel[joint->mJointNum] = offsetMat;
				

				std::queue<Joint*> que;

				size_t size = joint->mChildrens.size();
				for (int i = 0; i < size; ++i)
				{
					que.push(joint->mChildrens[i]);
				}

				while (!que.empty())
				{
					Joint* front = que.front();
					que.pop();
					size_t size = front->mChildrens.size();
					for (int i = 0; i < size; ++i)
					{
						que.push(front->mChildrens[i]);
					}
					ASSERT(front->mParent != nullptr && vec[front->mParent->mJointNum] != Matrix4x4::ZERO);

					offsetMat = Matrix4x4::IDENTITY;
					if (animatorComponent != nullptr)
					{
						if (!animatorComponent->GetCurrentTickBoneOffsetMat(front->mJointName, offsetMat))
						{
							offsetMat = front->mTransform.getMatrix();
						}
					}
					else if (animationComponent != nullptr)
					{
						if (!animationComponent->GetCurrentTickBoneOffsetMat(front->mJointName, offsetMat))
						{
							offsetMat = front->mTransform.getMatrix();
						}
					}
					else
					{
						offsetMat = front->mTransform.getMatrix();
					}

					vec[front->mJointNum] = front->mModleToLocalMat * offsetMat * localToModel[front->mParent->mJointNum];

					localToModel[front->mJointNum] = offsetMat * localToModel[front->mParent->mJointNum];
				}

				constans.BoneSum = vec.size();
				resource->UploadSkeletonGameObjectRenderResource(mRefMesh, mRefMaterials, constans, vec);

				return;
			}
		}
		while (false);

		constans.BoneSum = 0;
		resource->UploadGameObjectRenderResource(mRefMesh, mRefMaterials, constans);
	}

	GUID SkeletonMeshRendererComponent::GetRefSkeletonGUID()
	{
		return mRefSkeleton;
	}

	void SkeletonMeshRendererComponent::SetRefSkeleton(const GUID& guid)
	{
		mRefSkeleton = guid;
	}

	GUID SkeletonMeshRendererComponent::GetRefMeshGUID()
	{
		return mRefMesh;
	}

	std::vector<GUID>& SkeletonMeshRendererComponent::GetRefMaterials()
	{
		return mRefMaterials;
	}

	void SkeletonMeshRendererComponent::SetRefMesh(const GUID& guid)
	{
		mRefMesh = guid;
	}

	void SkeletonMeshRendererComponent::ClearRefMesh()
	{
		mRefMesh.SetNoVaild();
	}

	void SkeletonMeshRendererComponent::AddRefMaterial()
	{
		mRefMaterials.push_back(GUID());
	}

	void SkeletonMeshRendererComponent::RemoveRefMaterial()
	{
		ASSERT(mRefMaterials.size() > 0);

		mRefMaterials.pop_back();
	}

	void SkeletonMeshRendererComponent::SetRefMaterial(uint32_t index, const GUID& guid)
	{
		ASSERT(index < mRefMaterials.size());

		mRefMaterials[index] = guid;
	}

	void SkeletonMeshRendererComponent::SetNullMaterial(uint32_t index)
	{
		ASSERT(index < mRefMaterials.size());

		mRefMaterials[index].SetNoVaild();
	}

	void SkeletonMeshRendererComponent::Serialize(SerializerDataFrame& stream) const
	{
		stream << mRefMesh;

		stream << mRefSkeleton;

		stream << mRefMaterials;
	}

	bool SkeletonMeshRendererComponent::Deserialize(SerializerDataFrame& stream)
	{
		stream >> mRefMesh;

		stream >> mRefSkeleton;

		stream >> mRefMaterials;

		return true;
	}

	void SkeletonMeshRendererComponent::CloneData(GameObject* node)
	{
		SkeletonMeshRendererComponent* bluePrint = DynamicCast<SkeletonMeshRendererComponent>(node);

		mRefMesh = bluePrint->mRefMesh;

		mRefSkeleton = bluePrint->mRefSkeleton;

		mRefMaterials = bluePrint->mRefMaterials;
	}
}

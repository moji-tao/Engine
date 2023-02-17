#include "EngineRuntime/include/Core/Meta/Reflection.h"
#include "EngineRuntime/include/Function/Framework/Component/MeshRendererComponent.h"
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"

namespace Engine
{
	MeshRendererComponent::MeshRendererComponent()
		:Component()
	{
		mOrder = 1;
	}

	MeshRendererComponent::~MeshRendererComponent()
	{ }

	void MeshRendererComponent::Tick(float deltaTime)
	{
		auto transform_component = mParentObject->GetComponent<TransformComponent>();
		
		RenderResource* resource = RenderSystem::GetInstance()->GetRenderResource();

		ASSERT(resource != nullptr && transform_component != nullptr);

		ObjectConstants constans;
		transform_component->GetRenderMatrix(constans.World, constans.PreWorld);

		constans.InvWorld = constans.World.Inverse();

		resource->UploadGameObjectRenderResource(mRefMesh, mRefMaterials, constans);

	}

	GUID MeshRendererComponent::GetRefMeshGUID()
	{
		return mRefMesh;
	}

	std::vector<GUID>& MeshRendererComponent::GetRefMaterials()
	{
		return mRefMaterials;
	}

	void MeshRendererComponent::SetRefMesh(const GUID& guid)
	{
		mRefMesh = guid;
	}

	void MeshRendererComponent::ClearRefMesh()
	{
		mRefMesh.SetNoVaild();
	}

	void MeshRendererComponent::AddRefMaterial()
	{
		mRefMaterials.push_back(GUID());
	}

	void MeshRendererComponent::RemoveRefMaterial()
	{
		ASSERT(mRefMaterials.size() > 0);

		mRefMaterials.pop_back();
	}

	void MeshRendererComponent::SetRefMaterial(uint32_t index, const GUID& guid)
	{
		ASSERT(index < mRefMaterials.size());

		mRefMaterials[index] = guid;
	}

	void MeshRendererComponent::SetNullMaterial(uint32_t index)
	{
		ASSERT(index < mRefMaterials.size());

		mRefMaterials[index].SetNoVaild();
	}

	void MeshRendererComponent::Serialize(SerializerDataFrame& stream) const
	{
		stream << mRefMesh;

		stream << mRefMaterials;
	}

	bool MeshRendererComponent::Deserialize(SerializerDataFrame& stream)
	{
		stream >> mRefMesh;

		stream >> mRefMaterials;

		return true;
	}

	void MeshRendererComponent::CloneData(GameObject* node)
	{
		MeshRendererComponent* bluePrint = DynamicCast<MeshRendererComponent>(node);

		mRefMesh = bluePrint->mRefMesh;

		mRefMaterials = bluePrint->mRefMaterials;
	}
}

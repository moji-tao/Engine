#include "EngineRuntime/include/Function/Framework/Component/MeshRendererComponent.h"
#include "EngineRuntime/include/Core/Meta/Reflection.h"

namespace Engine
{
	IMPLEMENT_RTTI(MeshRendererComponent, Component);
	REGISTER_CLASS(MeshRendererComponent);

	MeshRendererComponent::MeshRendererComponent()
	{
	}
	MeshRendererComponent::~MeshRendererComponent()
	{
	}
	void MeshRendererComponent::Tick(float deltaTime)
	{
	}
	void MeshRendererComponent::Serialize(SerializerDataFrame& stream) const
	{
	}
	bool MeshRendererComponent::Deserialize(SerializerDataFrame& stream)
	{
		return false;
	}
}

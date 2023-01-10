#pragma once
#include "EngineRuntime/include/Function/Framework/Component/Component.h"

namespace Engine
{
	class MeshRendererComponent : public Component
	{
		DECLARE_RTTI;
	public:
		MeshRendererComponent();

		virtual ~MeshRendererComponent() override;

	public:
		virtual void Tick(float deltaTime) override;

	public:
		virtual void Serialize(SerializerDataFrame& stream) const override;

		virtual bool Deserialize(SerializerDataFrame& stream) override;

	protected:

	};
}

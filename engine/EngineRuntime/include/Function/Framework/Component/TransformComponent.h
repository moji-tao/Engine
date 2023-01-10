#pragma once
#include "EngineRuntime/include/Function/Framework/Component/Component.h"
#include "EngineRuntime/include/Core/Math/Transform.h"

namespace Engine
{
	class TransformComponent : public Component
	{
		DECLARE_RTTI;
	public:
		TransformComponent();

		virtual ~TransformComponent() override;

	public:
		virtual void Tick(float deltaTime) override;

	public:
		Vector3 GetPosition() const;

		Vector3 GetScale() const;

		Quaternion GetQuaternion() const;

		void SetPosition(const Vector3& position);

		void SetScale(const Vector3& scale);

		void SetQuaternion(const Quaternion& quaternion);

		Matrix4x4 GetMatrix() const;

	public:
		virtual void Serialize(SerializerDataFrame& stream) const override;

		virtual bool Deserialize(SerializerDataFrame& stream) override;

	protected:
		Transform mTransform;
	};
}

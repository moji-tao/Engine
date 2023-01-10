#include "EngineRuntime/include/Function/Framework/Component/TransformComponent.h"
#include "EngineRuntime/include/Core/Meta/Reflection.h"

namespace Engine
{
	IMPLEMENT_RTTI(TransformComponent, Component);
	REGISTER_CLASS(TransformComponent);

	TransformComponent::TransformComponent()
		:Component()
	{
		mTransform.position = Vector3::ZERO;
		mTransform.scale = Vector3::UNIT_SCALE;
		mTransform.rotation = Quaternion::IDENTITY;
	}

	TransformComponent::~TransformComponent()
	{ }

	void TransformComponent::Tick(float deltaTime)
	{

	}

	Vector3 TransformComponent::GetPosition() const
	{
		return mTransform.position;
	}

	Vector3 TransformComponent::GetScale() const
	{
		return mTransform.scale;
	}

	Quaternion TransformComponent::GetQuaternion() const
	{
		return mTransform.rotation;
	}

	void TransformComponent::SetPosition(const Vector3& position)
	{
		mTransform.position = position;
	}

	void TransformComponent::SetScale(const Vector3& scale)
	{
		mTransform.scale = scale;
	}

	void TransformComponent::SetQuaternion(const Quaternion& quaternion)
	{
		mTransform.rotation = quaternion;
	}

	Matrix4x4 TransformComponent::GetMatrix() const
	{
		return mTransform.getMatrix();
	}

	void TransformComponent::Serialize(SerializerDataFrame& stream) const
	{
		stream << mTransform;
	}

	bool TransformComponent::Deserialize(SerializerDataFrame& stream)
	{
		stream >> mTransform;

		return true;
	}
}


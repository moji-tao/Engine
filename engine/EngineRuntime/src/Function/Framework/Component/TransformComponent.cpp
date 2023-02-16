#include "EngineRuntime/include/Function/Framework/Component/TransformComponent.h"
#include "EngineRuntime/include/Core/Meta/Reflection.h"
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"

namespace Engine
{
	TransformComponent::TransformComponent()
		:Component()
	{
		mOrder = UINT_MAX;

		mTransform[mCurrentTick].position = Vector3::ZERO;
		mTransform[mCurrentTick].scale = Vector3::UNIT_SCALE;
		mTransform[mCurrentTick].rotation = Quaternion::IDENTITY;

		mTransform[mLastTick] = mTransform[mCurrentTick];
	}

	TransformComponent::~TransformComponent()
	{ }

	void TransformComponent::Tick(float deltaTime)
	{
		std::swap(mCurrentTick, mLastTick);

		mTransform[mCurrentTick] = mTransform[mLastTick];
	}

	void TransformComponent::Init(const Vector3& position, const Vector3& scale, const Quaternion& quaternion)
	{
		mTransform[0].position = position;
		mTransform[0].scale = scale;
		mTransform[0].rotation = quaternion;
		mTransform[1] = mTransform[0];
	}

	Vector3 TransformComponent::GetLocalPosition() const
	{
		return mTransform[mCurrentTick].position;
	}

	Vector3 TransformComponent::GetGlobalPosition() const
	{
		Vector3 result = mTransform[mCurrentTick].position;

		Actor* parent = mParentObject->GetParent();

		while (parent != nullptr)
		{
			result += parent->GetComponent<TransformComponent>()->GetLocalPosition();
			parent = parent->GetParent();
		}

		return result;
	}

	Vector3 TransformComponent::GetLocalScale() const
	{
		return mTransform[mCurrentTick].scale;
	}

	Vector3 TransformComponent::GetGlobalScale() const
	{
		Vector3 result = mTransform[mCurrentTick].scale;

		Actor* parent = mParentObject->GetParent();

		while (parent != nullptr)
		{
			result *= parent->GetComponent<TransformComponent>()->GetLocalScale();
			parent = parent->GetParent();
		}

		return result;
	}

	Quaternion TransformComponent::GetLocalQuaternion() const
	{
		return mTransform[mCurrentTick].rotation;
	}

	Quaternion TransformComponent::GetGlobalQuaternion() const
	{
		Quaternion result = mTransform[mCurrentTick].rotation;

		Actor* parent = mParentObject->GetParent();

		while (parent != nullptr)
		{
			result = parent->GetComponent<TransformComponent>()->GetLocalQuaternion() * result;
			parent = parent->GetParent();
		}

		return result;
	}

	Vector3& TransformComponent::GetLocalPosition()
	{
		return mTransform[mCurrentTick].position;
	}

	Vector3& TransformComponent::GetLocalScale()
	{
		return mTransform[mCurrentTick].scale;
	}

	Quaternion& TransformComponent::GetLocalQuaternion()
	{
		return mTransform[mCurrentTick].rotation;
	}

	void TransformComponent::SetPosition(const Vector3& position)
	{
		mTransform[mCurrentTick].position = position;
	}

	void TransformComponent::SetScale(const Vector3& scale)
	{
		mTransform[mCurrentTick].scale = scale;
	}

	void TransformComponent::SetQuaternion(const Quaternion& quaternion)
	{
		mTransform[mCurrentTick].rotation = quaternion;
	}

	void TransformComponent::SetGlobalTransform(Matrix4x4 mat)
	{
		Actor* parent = mParentObject->GetParent();

		if (parent != nullptr)
		{
			TransformComponent* parentComponent = parent->GetComponent<TransformComponent>();

			mat = mat * parentComponent->GetGlobalMatrix().Inverse();
		}

		mat.Decomposition(GetLocalPosition(), GetLocalScale(), GetLocalQuaternion());
	}

	Matrix4x4 TransformComponent::GetLocalMatrix() const
	{
		return mTransform[mCurrentTick].getMatrix();
	}

	Matrix4x4 TransformComponent::GetGlobalMatrix() const
	{
		Vector3 globalPosition = mTransform[mCurrentTick].position;
		Vector3 globalScale = mTransform[mCurrentTick].scale;
		Quaternion globalOrientation = mTransform[mCurrentTick].rotation;

		Actor* parent = mParentObject->GetParent();
		while (parent != nullptr)
		{
			TransformComponent* parentComponent = parent->GetComponent<TransformComponent>();
			globalPosition += parentComponent->GetLocalPosition();
			globalScale *= parentComponent->GetLocalScale();
			globalOrientation = parentComponent->GetLocalQuaternion() * globalOrientation;
			parent = parent->GetParent();
		}
		return Matrix4x4(globalPosition, globalScale, globalOrientation);
	}

	Matrix4x4 TransformComponent::GetLastGlobalMatrix() const
	{
		Vector3 globalLastPosition = mTransform[mLastTick].position;
		Vector3 globalLastScale = mTransform[mLastTick].scale;
		Quaternion globalLastOrientation = mTransform[mLastTick].rotation;

		Actor* parent = mParentObject->GetParent();

		while (parent != nullptr)
		{
			TransformComponent* parentComponent = parent->GetComponent<TransformComponent>();
			globalLastPosition += parentComponent->GetLastLocalPosition();
			globalLastScale *= parentComponent->GetLastLocalScale();
			globalLastOrientation = parentComponent->GetLastLocalQuaternion() * globalLastOrientation;
			parent = parent->GetParent();
		}
		return Matrix4x4(globalLastPosition, globalLastScale, globalLastOrientation);
	}

	void TransformComponent::GetRenderMatrix(Matrix4x4& currentMat, Matrix4x4& lastMat)
	{
		Vector3 globalPosition = mTransform[mCurrentTick].position;
		Vector3 globalScale = mTransform[mCurrentTick].scale;
		Quaternion globalOrientation = mTransform[mCurrentTick].rotation;
		Vector3 globalLastPosition = mTransform[mLastTick].position;
		Vector3 globalLastScale = mTransform[mLastTick].scale;
		Quaternion globalLastOrientation = mTransform[mLastTick].rotation;

		Actor* parent = mParentObject->GetParent();

		while (parent != nullptr)
		{
			TransformComponent* parentComponent = parent->GetComponent<TransformComponent>();

			globalPosition += parentComponent->GetLocalPosition();
			globalScale *= parentComponent->GetLocalScale();
			globalOrientation = parentComponent->GetLocalQuaternion() * globalOrientation;
			globalLastPosition += parentComponent->GetLastLocalPosition();
			globalLastScale *= parentComponent->GetLastLocalScale();
			globalLastOrientation = parentComponent->GetLastLocalQuaternion() * globalLastOrientation;

			parent = parent->GetParent();
		}

		currentMat = Matrix4x4(globalPosition, globalScale, globalOrientation);

		lastMat = Matrix4x4(globalLastPosition, globalLastScale, globalLastOrientation);
	}

	Vector3 TransformComponent::GetLastLocalPosition() const
	{
		return mTransform[mLastTick].position;
	}

	Vector3 TransformComponent::GetLastGlobalPosition() const
	{
		Vector3 result = mTransform[mLastTick].position;

		Actor* parent = mParentObject->GetParent();

		while (parent != nullptr)
		{
			result += parent->GetComponent<TransformComponent>()->GetLastLocalPosition();
			parent = parent->GetParent();
		}

		return result;
	}

	Vector3 TransformComponent::GetLastLocalScale() const
	{
		return mTransform[mLastTick].scale;
	}

	Vector3 TransformComponent::GetLastGlobalScale() const
	{
		Vector3 result = mTransform[mLastTick].scale;

		Actor* parent = mParentObject->GetParent();

		while (parent != nullptr)
		{
			result *= parent->GetComponent<TransformComponent>()->GetLastLocalScale();
			parent = parent->GetParent();
		}

		return result;
	}

	Quaternion TransformComponent::GetLastLocalQuaternion() const
	{
		return mTransform[mLastTick].rotation;
	}

	Quaternion TransformComponent::GetLastGlobalQuaternion() const
	{
		Quaternion result = mTransform[mLastTick].rotation;

		Actor* parent = mParentObject->GetParent();

		while (parent != nullptr)
		{
			result = parent->GetComponent<TransformComponent>()->GetLocalQuaternion() * result;
			parent = parent->GetParent();
		}

		return result;
	}

	Vector3& TransformComponent::GetLastLocalPosition()
	{
		return mTransform[mLastTick].position;
	}

	Vector3& TransformComponent::GetLastLocalScale()
	{
		return mTransform[mLastTick].scale;
	}

	Quaternion& TransformComponent::GetLastLocalQuaternion()
	{
		return mTransform[mLastTick].rotation;
	}

	void TransformComponent::Serialize(SerializerDataFrame& stream) const
	{
		stream << mTransform[mCurrentTick];
	}

	bool TransformComponent::Deserialize(SerializerDataFrame& stream)
	{
		mCurrentTick = 0;
		mLastTick = 1;

		stream >> mTransform[mCurrentTick];
		mTransform[mLastTick] = mTransform[mCurrentTick];

		return true;
	}

	void TransformComponent::CloneData(GameObject* node)
	{
		TransformComponent* bluePrint = DynamicCast<TransformComponent>(node);

		mTransform[mCurrentTick] = bluePrint->mTransform[bluePrint->mCurrentTick];
		mTransform[mLastTick] = mTransform[mCurrentTick];
	}
}


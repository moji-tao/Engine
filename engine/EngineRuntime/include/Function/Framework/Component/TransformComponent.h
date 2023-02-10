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
		void Init(const Vector3& position, const Vector3& scale, const Quaternion& quaternion);

		Vector3 GetLocalPosition() const;

		Vector3 GetGlobalPosition() const;

		Vector3 GetLocalScale() const;

		Vector3 GetGlobalScale() const;

		Quaternion GetLocalQuaternion() const;

		Quaternion GetGlobalQuaternion() const;

		Vector3& GetLocalPosition();

		Vector3& GetLocalScale();

		Quaternion& GetLocalQuaternion();

		void SetPosition(const Vector3& position);

		void SetScale(const Vector3& scale);

		void SetQuaternion(const Quaternion& quaternion);

		void SetGlobalTransform(Matrix4x4 mat);

		Matrix4x4 GetLocalMatrix() const;

		Matrix4x4 GetGlobalMatrix() const;

		Matrix4x4 GetLastGlobalMatrix() const;

		void GetRenderMatrix(Matrix4x4& currentMat, Matrix4x4& lastMat);

	private:
		Vector3 GetLastLocalPosition() const;

		Vector3 GetLastGlobalPosition() const;

		Vector3 GetLastLocalScale() const;

		Vector3 GetLastGlobalScale() const;

		Quaternion GetLastLocalQuaternion() const;

		Quaternion GetLastGlobalQuaternion() const;

		Vector3& GetLastLocalPosition();

		Vector3& GetLastLocalScale();

		Quaternion& GetLastLocalQuaternion();

	public:
		virtual void Serialize(SerializerDataFrame& stream) const override;

		virtual bool Deserialize(SerializerDataFrame& stream) override;

		virtual void CloneData(GameObject* node) override;

	protected:
		Transform mTransform[2];

		short mCurrentTick = 0;
		short mLastTick = 1;
	};
}

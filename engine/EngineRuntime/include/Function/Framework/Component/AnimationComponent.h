#pragma once
#include "EngineRuntime/include/Function/Framework/Component/Component.h"

namespace Engine
{
	class AnimationComponent : public Component
	{
		DECLARE_RTTI;
	public:
		AnimationComponent();

		virtual ~AnimationComponent() override;

	public:
		virtual void Tick(float deltaTime) override;

	public:
		bool GetCurrentTickBoneOffsetMat(const std::string& boneName, Matrix4x4& mat);

		bool GetCurrentTickBoneOffsetTransform(const std::string& boneName , Transform& transform);

		GUID GetAnimationGuid();

		void SetAnimationGuid(const GUID& guid);

	public:
		virtual void Serialize(SerializerDataFrame& stream) const override;

		virtual bool Deserialize(SerializerDataFrame& stream) override;

		virtual void CloneData(GameObject* node) override;

	protected:
		GUID mRefAnimationRes;

		double mCurrentTickAnimationTime;
	};
}

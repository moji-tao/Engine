#pragma once
#include "EngineRuntime/include/Function/Framework/Component/Component.h"

namespace Engine
{
	class AnimatorComponent : public Component
	{
		DECLARE_RTTI;
	public:
		AnimatorComponent();

		virtual ~AnimatorComponent() override;

	public:
		virtual void Awake() override;

		virtual void Tick(float deltaTime) override;

		virtual void OnDestroy() override;

		virtual void OnEnable() override;

	public:
		/*
		void SetBool(const std::string& name, bool value);

		void SetFloat(const std::string& name, float value);

		void SetTrigger(const std::string& name, bool value);

		bool GetBool(const std::string& name);

		float GetFloat(const std::string& name);
		*/

		std::vector<std::pair<GUID, float>>& GetAnimationBlending();

		void SetAnimation(uint32_t index, const GUID& animationGuid);

		void SetAnimationWeight(uint32_t index, float weight);

		void AddAnimation();

		void RemoveAnimation();

		void SortAnimation();

	public:
		bool GetCurrentTickBoneOffsetMat(const std::string& boneName, Matrix4x4& mat);

		bool GetCurrentTickBoneOffsetTransform(const std::string& boneName, Transform& transform);

	public:
		virtual void Serialize(SerializerDataFrame& stream) const override;

		virtual bool Deserialize(SerializerDataFrame& stream) override;

		virtual void CloneData(GameObject* node) override;

	public:
		float mWeight = 0.0f;

	protected:
		std::vector<std::pair<GUID, float>> mAnimationBlending;


		// runtime Calc

		GUID mFirstAnimationClipRes;
		GUID mSecondAnimationClipRes;

		// mFirstWeight + mSecondWeight = 1.0f
		float mFirstWeight;
		float mSecondWeight;

		double mCurrentTickAnimationTime = 0.0;
	};
}

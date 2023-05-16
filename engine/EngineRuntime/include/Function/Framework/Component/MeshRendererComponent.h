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
		virtual void Awake() override;

		virtual void Tick(float deltaTime) override;

		virtual void OnDestroy() override;

		virtual void OnEnable() override;

	public:
		GUID GetRefMeshGUID();

		std::vector<GUID>& GetRefMaterials();

		void SetRefMesh(const GUID& guid);

		void ClearRefMesh();

		void AddRefMaterial();

		void RemoveRefMaterial();

		void SetRefMaterial(uint32_t index, const GUID& guid);

		void SetNullMaterial(uint32_t index);

	public:
		virtual void Serialize(SerializerDataFrame& stream) const override;

		virtual bool Deserialize(SerializerDataFrame& stream) override;

		virtual void CloneData(GameObject* node) override;

	protected:
		GUID mRefMesh;

		std::vector<GUID> mRefMaterials;
	};
}

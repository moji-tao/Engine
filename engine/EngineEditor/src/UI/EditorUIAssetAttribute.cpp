#include "EngineEditor/include/UI/EditorUIAssetAttribute.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"

namespace Editor
{
	EditorUIAssetAttributePass::EditorUIAssetAttributePass()
		:EditorUIPassBase()
	{ }

	EditorUIAssetAttributePass::~EditorUIAssetAttributePass()
	{ }


	void EditorUIAssetAttributePass::Initialize(EditorUIMessage* messageBox, Engine::ImGuiDevice* device, EngineEditor* editor)
	{
		EditorUIPassBase::Initialize(messageBox, device, editor);

	}

	void EditorUIAssetAttributePass::ShowUI()
	{
		if (!mIsOpen)
			return;

		ImGui::Begin("属性", &mIsOpen, ImGuiWindowFlags_NoCollapse);

		mShowAssetGuid = mMessageBox->GetProjectSelectFile();

		do
		{
			if (!mShowAssetGuid.IsVaild())
			{
				break;
			}

			Object* resource = Engine::AssetManager::GetInstance()->LoadResource(mShowAssetGuid);

			if (resource->GetType().mName == "MaterialData")
			{
				ImGui::Text("材质属性");
				Engine::MaterialData* material = Engine::DynamicCast<Engine::MaterialData>(resource);

				ASSERT(material != nullptr);

				ImGui::ColorEdit4("颜色##AssetAttribute_Albedo", material->Albedo.ptr());
				ShowDragGuidAsset(material->RefAlbedoTexture, "漫反射贴图##AssetAttribute_AlbedoTexture");
				ImGui::SameLine();
				if (ImGui::Button("清除##AssetAttribute_Albedo_ClearButton"))
				{
					material->RefAlbedoTexture = Engine::GUID();
				}
				ImGui::Separator();
				ShowDragGuidAsset(material->RefNormalTexture, "法线贴图##AssetAttribute_NormalTexture");
				ImGui::SameLine();
				if (ImGui::Button("清除##AssetAttribute_Normal_ClearButton"))
				{
					material->RefNormalTexture = Engine::GUID();
				}
				ImGui::Separator();
				ImGui::DragFloat("金属度##AssetAttribute_Metallic", &material->Metallic, 0.001f, 0.0f, 1.0f);
				ShowDragGuidAsset(material->RefMetallicTexture, "金属度贴图##AssetAttribute_MetallicTexture");
				ImGui::SameLine();
				if (ImGui::Button("清除##AssetAttribute_Metallic_ClearButton"))
				{
					material->RefMetallicTexture = Engine::GUID();
				}
				ImGui::Separator();
				ImGui::DragFloat("粗糙度##AssetAttribute_Roughness", &material->Roughness, 0.001f, 0.0f, 1.0f);
				ShowDragGuidAsset(material->RefRoughnessTexture, "粗糙度贴图##AssetAttribute_RoughnessTexture");
				ImGui::SameLine();
				if (ImGui::Button("清除##AssetAttribute_Roughness_ClearButton"))
				{
					material->RefRoughnessTexture = Engine::GUID();
				}
				ImGui::Separator();
				ImGui::DragFloat3("自发光##AssetAttribute_Emissive", material->Emissive.ptr(), 0.001f, 0.0f, 1.0f);

				ImGui::Separator();

				if (ImGui::Button("保存####AssetAttribute_Material_SaveButton"))
				{
					auto pathses = Engine::AssetManager::GetInstance()->GetAssetPathFormAssetGuid(material->GetGuid());
					LOG_INFO("保存材质 guid:{0} 路径:{1}", material->GetGuid().Data(), pathses.generic_string().c_str());

					Engine::SerializerDataFrame frame;

					frame << *material;

					frame.Save(pathses);
				}
			}
		} while (false);

		ImGui::End();
	}

	void EditorUIAssetAttributePass::ShowDragGuidAsset(Engine::GUID& assetGuid, const std::string& name)
	{
		std::string value = "空";
		if (assetGuid.IsVaild())
		{
			auto ph = Engine::AssetManager::GetInstance()->GetAssetPathFormAssetGuid(assetGuid);
			value = ph.filename().generic_string();
		}
		ImGui::LabelText(name.c_str(), value.c_str());

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(UIProjectFileDrag))
			{
				AssetFile* file = *(AssetFile**)payload->Data;
				std::filesystem::path fileName(file->mName);
				if (fileName.extension() == ".texture")
				{
					Engine::GUID guid = file->GetAssetGuid();
					if (guid.IsVaild())
					{
						assetGuid = guid;
					}
					else
					{
						LOG_ERROR("没有获取到贴图 {0}", file->mName.c_str());
					}
				}
				else
				{
					LOG_ERROR("该文件不可以转换为贴图 {0}", file->mName.c_str());
				}
			}
		}
	}
}

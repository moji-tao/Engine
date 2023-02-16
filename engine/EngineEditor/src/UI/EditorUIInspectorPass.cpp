#include <imgui/imgui.h>
#include "EngineEditor/include/UI/EditorUIInspectorPass.h"
#include "EngineRuntime/include/Core/Math/Angle.h"
#include "EngineRuntime/include/Function/Framework/Component/MeshRendererComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/TransformComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/DirectionalLightComponent.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"

namespace Editor
{
	EditorUIInspectorPass::EditorUIInspectorPass()
		:EditorUIPassBase()
	{
		mComponentUIMap["TransformComponent"] = [this](Engine::Component* component)
		{
			Engine::TransformComponent* transformComponent = dynamic_cast<Engine::TransformComponent*>(component);
			ASSERT(transformComponent != nullptr);
			
			Engine::Vector3& position = transformComponent->GetLocalPosition();
			Engine::Vector3& scale = transformComponent->GetLocalScale();
			Engine::Quaternion& quaternion = transformComponent->GetLocalQuaternion();

			if (mRest || !mEditor->mIsEditorMode)
			{
				eural = Engine::Vector3(quaternion.GetPitch().ValueDegrees(), quaternion.GetYaw().ValueDegrees(), quaternion.GetRoll().ValueDegrees());
				mRest = false;
			}
			
			DrawVec3Control("Position", position);
			DrawVec3Control("Rotation", eural);
			DrawVec3Control("Scale", scale);

			quaternion = Engine::Quaternion(Engine::Degree(eural[2]), Engine::Degree(eural[0]), Engine::Degree(eural[1]));
		};

		mComponentUIMap["MeshRendererComponent"] = [this](Engine::Component* component)
		{
			Engine::MeshRendererComponent* meshRendererComponent = dynamic_cast<Engine::MeshRendererComponent*>(component);
			ASSERT(meshRendererComponent != nullptr);

			Engine::GUID refMeshGuid = meshRendererComponent->GetRefMeshGUID();
			std::vector<Engine::GUID>& refMateruals = meshRendererComponent->GetRefMaterials();

			if (refMeshGuid.IsVaild())
			{
				memcpy(mRefMeshName, refMeshGuid.Data(), refMeshGuid.Size() + 1);
			}
			else
			{
				memset(mRefMeshName, 0x00, sizeof(mRefMeshName));
			}

			ImGui::LabelText("Mesh##Inspector", "%s", mRefMeshName);

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(UIProjectFileDrag))
				{
					AssetFile* file = *(AssetFile**)payload->Data;
					std::filesystem::path fileName(file->mName);
					if (fileName.extension() == ".mesh")
					{
						std::filesystem::path filePath = file->GetAssetFilePath();
						Engine::Mesh* mesh = Engine::AssetManager::GetInstance()->LoadResource<Engine::Mesh>(filePath);
						meshRendererComponent->SetRefMesh(mesh->GetGuid());
					}
					else
					{
						LOG_ERROR("该文件不可以转换为Mesh {0}", file->mName.c_str());
					}
				}
			}

			bool isOpen = ImGui::TreeNode("Materials##Inspector");
			ImGui::SameLine();
			ImGui::Text("  %d", refMateruals.size());
			if (isOpen)
			{
				for (int i = 0; i < refMateruals.size(); ++i)
				{
					std::string value = "空";
					if (refMateruals[i].IsVaild())
					{
						auto ph = Engine::AssetManager::GetInstance()->GetAssetPathFormAssetGuid(refMateruals[i]);
						value = ph.filename().generic_string();
					}
					ImGui::LabelText(("Material " + std::to_string(i) + "##Inspector").c_str(), "%s", value.c_str());
					
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(UIProjectFileDrag))
						{
							AssetFile* file = *(AssetFile**)payload->Data;
							std::filesystem::path fileName(file->mName);
							if (fileName.extension() == ".material")
							{
								std::filesystem::path filePath = file->GetAssetFilePath();
								Engine::MaterialData* material = Engine::AssetManager::GetInstance()->LoadResource<Engine::MaterialData>(filePath);
								meshRendererComponent->SetRefMaterial(i, material->GetGuid());
							}
							else
							{
								LOG_ERROR("该文件不可以转换为Material {0}", file->mName.c_str());
							}
						}
					}

				}
				ImGui::TreePop();
			}

			if (ImGui::Button("追加材质"))
			{
				meshRendererComponent->AddRefMaterial();
			}
			if (refMateruals.size() != 0)
			{
				ImGui::SameLine();
				if (ImGui::Button("删除材质"))
				{
					meshRendererComponent->RemoveRefMaterial();
				}
			}
		};

		mComponentUIMap["DirectionalLightComponent"] = [this](Engine::Component* component)
		{
			Engine::DirectionalLightComponent* directionalLightComponent = dynamic_cast<Engine::DirectionalLightComponent*>(component);
			ASSERT(directionalLightComponent != nullptr);

			ImGui::ColorEdit4("Color", directionalLightComponent->mColor.ptr());
			
			ImGui::DragFloat("##DirectionalLight_Intensity_Drag", &directionalLightComponent->mIntensity, 0.01f, 0.0f, 0.0f, "光强", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SameLine();
			ImGui::InputFloat("##DirectionalLight_Intensity_Input", &directionalLightComponent->mIntensity);

			ImGui::Checkbox("阴影##DirectionalLight_ShowShadow", &directionalLightComponent->mShadow);
		};
	}

	EditorUIInspectorPass::~EditorUIInspectorPass()
	{

	}

	void EditorUIInspectorPass::Initialize(EditorUIMessage* messageBox, Engine::ImGuiDevice* device, EngineEditor* editor)
	{
		EditorUIPassBase::Initialize(messageBox, device, editor);
	}

	void EditorUIInspectorPass::ShowUI()
	{
		if (!mIsOpen)
			return;

		mActor = mMessageBox->GetInspectorShowActor();
		ImGui::Begin("检查器", &mIsOpen, ImGuiWindowFlags_NoCollapse);

		if (mActor != nullptr)
		{
			if(mLastActor != mActor)
			{
				memset(mNameBuffer, 0x00, sizeof(mNameBuffer));
				mLastActor = mActor;
				std::string actorName = mActor->GetActorName();
				memcpy(mNameBuffer, actorName.c_str(), actorName.size());
				mRest = true;
			}
			ImGui::Text("Name: ");
			ImGui::SameLine();
			if (ImGui::InputText("##InspectorName", mNameBuffer, sizeof(mNameBuffer)))
			{
				LOG_INFO("ddddddd");
			}

			ImGui::Text("=========================================");

			std::vector<Engine::Component*>& components = mActor->GetAllComponent();

			for (auto it : components)
			{
				std::string componentName = it->GetType().mName;
				if(ImGui::TreeNode(componentName.c_str()))
				{
					mComponentUIMap[componentName](it);

					ImGui::TreePop();
				}
			}

			ImGui::Separator();
			
			ImGuiStyle& style = ImGui::GetStyle();

			float size = ImGui::CalcTextSize("AddComponent").x + style.FramePadding.x * 2.0f;
			float avail = ImGui::GetContentRegionAvail().x;

			float off = (avail - size) * 0.5f;
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

			if (ImGui::Button("AddComponent"))
			{
				ImGui::OpenPopup("my_toggle_popup");
			}

			if (ImGui::BeginPopup("my_toggle_popup"))
			{
				/*
				static ImGuiTextFilter filter;
				filter.Draw();
				const char* lines[] = { "aaa1.c", "bbb1.c", "ccc1.c", "aaa2.cpp", "bbb2.cpp", "ccc2.cpp", "abc.h", "hello, world" };
				for (int i = 0; i < IM_ARRAYSIZE(lines); i++)
					if (filter.PassFilter(lines[i]))
						ImGui::BulletText("%s", lines[i]);
				*/
				for (const std::string& c : Engine::Component::ALLInstanceComponentName)
				{
					if (ImGui::Button(c.c_str()))
					{
						mActor->AddComponent(c);
					}
				}

				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}

	void EditorUIInspectorPass::DrawVec3Control(const std::string& label, Engine::Vector3& values)
	{
		ImGui::PushID(label.c_str());

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 2));
		ImGui::PushItemWidth(200);
		ImGui::Text("%-10s", label.c_str());
		ImGui::PopItemWidth();

		ImGui::PushItemWidth(20);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.204f, 0.204f, 0.204f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.204f, 0.204f, 0.204f, 1.0f));

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values[0], 0.1f, 0.0f, 0.0f, "X", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SameLine(0);

		ImGui::PushItemWidth(80);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.204f, 0.204f, 0.204f, 1.0f));
		ImGui::InputFloat("##X_Input", &values[0]);
		ImGui::PopStyleColor();
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values[1], 0.1f, 0.0f, 0.0f, "Y", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SameLine();

		ImGui::PushItemWidth(80);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.204f, 0.204f, 0.204f, 1.0f));
		ImGui::InputFloat("##Y_Input", &values[1]);
		ImGui::PopStyleColor();
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values[2], 0.1f, 0.0f, 0.0f, "Z", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SameLine();

		ImGui::PushItemWidth(80);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.204f, 0.204f, 0.204f, 1.0f));
		ImGui::InputFloat("##Z_Input", &values[2]);
		ImGui::PopStyleColor();
		ImGui::PopItemWidth();

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::PopID();
	}
}

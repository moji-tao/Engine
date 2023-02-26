#include <imgui/imgui.h>
#include "EngineEditor/include/UI/EditorUIInspectorPass.h"
#include "EngineEditor/include/UI/ImGuiExtensions/DrawVec3Control.h"
#include "EngineRuntime/include/Core/Math/Angle.h"
#include "EngineRuntime/include/Function/Framework/Component/AnimationComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/AnimatorComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/MeshRendererComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/TransformComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/DirectionalLightComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/PointLightComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/SkeletonMeshRendererComponent.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/AnimationClip.h"

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

			ImGui::DrawVec3Control("Position", position);
			ImGui::DrawVec3Control("Rotation", eural);
			ImGui::DrawVec3Control("Scale", scale);

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

			ImGui::LabelText("Mesh##Inspector_MeshRendererComponent", "%s", mRefMeshName);

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

			bool isOpen = ImGui::TreeNode("Materials##Inspector_MeshRendererComponent");
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

		mComponentUIMap["SkeletonMeshRendererComponent"] = [this](Engine::Component* component)
		{
			Engine::SkeletonMeshRendererComponent* skeletonMeshRendererComponent = dynamic_cast<Engine::SkeletonMeshRendererComponent*>(component);
			ASSERT(skeletonMeshRendererComponent != nullptr);

			Engine::GUID refMeshGuid = skeletonMeshRendererComponent->GetRefMeshGUID();
			Engine::GUID refSkeletonGuid = skeletonMeshRendererComponent->GetRefSkeletonGUID();
			std::vector<Engine::GUID>& refMateruals = skeletonMeshRendererComponent->GetRefMaterials();

			if (refMeshGuid.IsVaild())
			{
				memcpy(mRefMeshName, refMeshGuid.Data(), refMeshGuid.Size() + 1);
			}
			else
			{
				memset(mRefMeshName, 0x00, sizeof(mRefMeshName));
			}

			if (refSkeletonGuid.IsVaild())
			{
				memcpy(mRefSkeletonName, refSkeletonGuid.Data(), refSkeletonGuid.Size() + 1);
			}
			else
			{
				memset(mRefSkeletonName, 0x00, sizeof(mRefSkeletonName));
			}

			ImGui::LabelText("Mesh##Inspector_SkeletonMeshRendererComponent", "%s", mRefMeshName);

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
						skeletonMeshRendererComponent->SetRefMesh(mesh->GetGuid());
					}
					else
					{
						LOG_ERROR("该文件不可以转换为Mesh {0}", file->mName.c_str());
					}
				}
			}

			ImGui::LabelText("Skeleton##Inspector_SkeletonMeshRendererComponent", "%s", mRefSkeletonName);

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(UIProjectFileDrag))
				{
					AssetFile* file = *(AssetFile**)payload->Data;
					std::filesystem::path fileName(file->mName);
					if (fileName.extension() == ".skeleton")
					{
						std::filesystem::path filePath = file->GetAssetFilePath();
						Engine::SkeletonData* skeleton = Engine::AssetManager::GetInstance()->LoadResource<Engine::SkeletonData>(filePath);
						skeletonMeshRendererComponent->SetRefSkeleton(skeleton->GetGuid());
					}
					else
					{
						LOG_ERROR("该文件不可以转换为Skeleton {0}", file->mName.c_str());
					}
				}
			}

			bool isOpen = ImGui::TreeNode("Materials##Inspector_SkeletonMeshRendererComponent");
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
								skeletonMeshRendererComponent->SetRefMaterial(i, material->GetGuid());
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
				skeletonMeshRendererComponent->AddRefMaterial();
			}
			if (refMateruals.size() != 0)
			{
				ImGui::SameLine();
				if (ImGui::Button("删除材质"))
				{
					skeletonMeshRendererComponent->RemoveRefMaterial();
				}
			}
		};

		mComponentUIMap["DirectionalLightComponent"] = [this](Engine::Component* component)
		{
			Engine::DirectionalLightComponent* directionalLightComponent = dynamic_cast<Engine::DirectionalLightComponent*>(component);
			ASSERT(directionalLightComponent != nullptr);

			ImGui::ColorEdit4("Color##DirectionalLight_Color", directionalLightComponent->mColor.ptr());
			
			ImGui::DragFloat("##DirectionalLight_Intensity_Drag", &directionalLightComponent->mIntensity, 0.01f, 0.0f, 0.0f, "光强", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SameLine();
			ImGui::InputFloat("##DirectionalLight_Intensity_Input", &directionalLightComponent->mIntensity);

			ImGui::Checkbox("阴影##DirectionalLight_ShowShadow", &directionalLightComponent->mShadow);
		};

		mComponentUIMap["PointLightComponent"] = [this](Engine::Component* component)
		{
			Engine::PointLightComponent* pointLightComponent = dynamic_cast<Engine::PointLightComponent*>(component);
			ASSERT(pointLightComponent != nullptr);

			ImGui::ColorEdit4("Color##PointLightComponent_Color", pointLightComponent->mColor.ptr());

			ImGui::DragFloat("##PointLightComponent_Intensity_Drag", &pointLightComponent->mIntensity, 0.01f, 0.001f, 0.0f, "光强", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SameLine();
			ImGui::InputFloat("##PointLightComponent_Intensity_Input", &pointLightComponent->mIntensity);

			ImGui::DragFloat("##PointLightComponent_Range_Drag", &pointLightComponent->mRange, 0.01f, 0.001f, 0.0f, "半径", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SameLine();
			ImGui::InputFloat("##PointLightComponent_Range_Input", &pointLightComponent->mRange);

			ImGui::Checkbox("阴影##PointLightComponent_ShowShadow", &pointLightComponent->mShadow);
		};

		mComponentUIMap["AnimationComponent"] = [this](Engine::Component* component)
		{
			Engine::AnimationComponent* animationComponent = dynamic_cast<Engine::AnimationComponent*>(component);
			ASSERT(animationComponent != nullptr);

			Engine::GUID refAnimationGuid = animationComponent->GetAnimationGuid();

			if (refAnimationGuid.IsVaild())
			{
				memcpy(mRefAnimationName, refAnimationGuid.Data(), refAnimationGuid.Size() + 1);
			}
			else
			{
				memset(mRefAnimationName, 0x00, sizeof(mRefAnimationName));
			}

			ImGui::LabelText("Animation##Inspector_AnimationComponent", "%s", mRefAnimationName);

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(UIProjectFileDrag))
				{
					AssetFile* file = *(AssetFile**)payload->Data;
					std::filesystem::path fileName(file->mName);
					if (fileName.extension() == ".animation")
					{
						std::filesystem::path filePath = file->GetAssetFilePath();
						Engine::AnimationClip* animationClip = Engine::AssetManager::GetInstance()->LoadResource<Engine::AnimationClip>(filePath);
						animationComponent->SetAnimationGuid(animationClip->GetGuid());
					}
					else
					{
						LOG_ERROR("该文件不可以转换为AnimationClip {0}", file->mName.c_str());
					}
				}
			}
		};

		mComponentUIMap["AnimatorComponent"] = [this](Engine::Component* component)
		{
			Engine::AnimatorComponent* animatorComponent = dynamic_cast<Engine::AnimatorComponent*>(component);
			ASSERT(animatorComponent != nullptr);

			ImGui::DragFloat("权重##AnimatorComponent_Weight", &animatorComponent->mWeight, 0.01f);

			std::vector<std::pair<Engine::GUID, float>>& animations = animatorComponent->GetAnimationBlending();

			ImGui::Text("Animation");
			bool b = false;
			for (size_t i = 0; i < animations.size(); i++)
			{
				std::string value = "空";
				if (animations[i].first.IsVaild())
				{
					auto ph = Engine::AssetManager::GetInstance()->GetAssetPathFormAssetGuid(animations[i].first);
					value = ph.filename().generic_string();
				}
				ImGui::LabelText(("AnimatorBlending " + std::to_string(i) + "##Inspector_AnimatorComponent").c_str(), "%s", value.c_str());

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(UIProjectFileDrag))
					{
						AssetFile* file = *(AssetFile**)payload->Data;
						std::filesystem::path fileName(file->mName);
						if (fileName.extension() == ".animation")
						{
							std::filesystem::path filePath = file->GetAssetFilePath();
							Engine::AnimationClip* animationClip = Engine::AssetManager::GetInstance()->LoadResource<Engine::AnimationClip>(filePath);
							animatorComponent->SetAnimation(i, animationClip->GetGuid());
						}
						else
						{
							LOG_ERROR("该文件不可以转换为AnimationClip {0}", file->mName.c_str());
						}
					}
				}

				if (ImGui::DragFloat(("动画权重 " + std::to_string(i) + "##AnimatorComponent_Weight").c_str(), &animations[i].second, 0.01f))
				{
					b = true;
				}
			}
			if (b)
			{
				animatorComponent->SortAnimation();
			}

			if (ImGui::Button("追加动画"))
			{
				animatorComponent->AddAnimation();
			}
			if (animations.size() != 0)
			{
				ImGui::SameLine();
				if (ImGui::Button("删除动画"))
				{
					animatorComponent->RemoveAnimation();
				}
			}
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
}

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <imgui/imgui.h>
#include "EngineEditor/include/UI/SubFrame/EditorRightMenu.h"
#include "EngineRuntime/include/Core/Base/macro.h"

namespace Editor
{
	std::shared_ptr<RightMenuContext> ReadXml(const boost::property_tree::ptree& pt);

	std::shared_ptr<RightMenuUI> EditorRightMenu::CreateRightMenu(const std::filesystem::path& filePath, const std::string& menuName)
	{
		boost::property_tree::ptree pt;
		boost::property_tree::xml_parser::read_xml(filePath.generic_string().c_str(), pt);
		auto pt2 = pt.get_child("RightMenuUI.Context");

		std::shared_ptr<RightMenuUI> result = std::make_shared<RightMenuUI>();
		result->MenuName = menuName;
		result->Context = ReadXml(pt2);

		return result;
	}

	std::shared_ptr<RightMenuContext> ReadXml(const boost::property_tree::ptree& pt)
	{
		std::shared_ptr<RightMenuContext> context = std::make_shared<RightMenuContext>();

		for (auto it = pt.begin(); it != pt.end(); ++it)
		{
			std::shared_ptr<RightMenuItemBase> item;
			if (it->first == "SubMenu")
			{
				std::shared_ptr<RightMenuItemSubMenu> item;
				std::string name = it->second.get<std::string>("<xmlattr>.Name");
				std::string enable = it->second.get<std::string>("<xmlattr>.Enable");

				if (enable[0] == '$')
				{
					item = std::make_shared<RightMenuItemSubMenuVariable>(name, std::string(enable.begin() + 1, enable.end()));
				}
				else if (enable == "true")
				{
					item = std::make_shared<RightMenuItemSubMenuConstant>(name);
				}
				else if (enable == "false")
				{
					item = std::make_shared<RightMenuItemSubMenuConstant>(name);
				}
				else
				{
					ASSERT(0);
				}
				item->Next = ReadXml(it->second.get_child("Context"));
				context->List.emplace_back(item);
			}
			else if (it->first == "Item")
			{
				std::shared_ptr<RightMenuItemClick> item;
				std::string name = it->second.get<std::string>("<xmlattr>.Name");
				std::string enable = it->second.get<std::string>("<xmlattr>.Enable");
				std::string method = it->second.get<std::string>("<xmlattr>.Method");

				if (enable[0] == '$')
				{
					item = std::make_shared<RightMenuItemClickVariable>(name, std::string(enable.begin() + 1, enable.end()), method);
				}
				else if (enable == "true")
				{
					item = std::make_shared<RightMenuItemClickConstant>(name, method);
				}
				else if (enable == "false")
				{
					item = std::make_shared<RightMenuItemClickConstant>(name, method);
				}
				else
				{
					ASSERT(0);
				}

				context->List.emplace_back(item);
			}
			else if (it->first == "Separator")
			{
				std::shared_ptr<RightMenuItemSeparator> item = std::make_shared<RightMenuItemSeparator>();
				context->List.emplace_back(item);
			}
			else
			{
				ASSERT(0, "未定义标签");
			}
		}

		return context;
	}

	void ShowRightMenuContext(RightMenuContext* menu, Engine::Object* uiPass)
	{
		std::vector<std::shared_ptr<RightMenuItemBase>>& List = menu->List;
		for (unsigned i = 0; i < menu->List.size(); ++i)
		{
			List[i]->Invoke(uiPass);
		}
	}

	void EditorRightMenu::DrawRightMenu(RightMenuUI* menu, Engine::Object* uiPass)
	{
		if (ImGui::BeginPopup(menu->MenuName.c_str()))
		{
			ShowRightMenuContext(menu->Context.get(), uiPass);

			ImGui::EndPopup();
		}
	}

	void EditorRightMenu::ShowRightMenu(RightMenuUI* menu)
	{
		ImGui::OpenPopup(menu->MenuName.c_str());
	}

	bool EditorRightMenu::RightMenuIsPopup(RightMenuUI* menu)
	{
		return ImGui::IsPopupOpen(menu->MenuName.c_str());
	}

	RightMenuItemConstant::RightMenuItemConstant(const std::string& name)
		:ItemName(name)
	{ }

	RightMenuItemVariable::RightMenuItemVariable(const std::string& name, const std::string& enableName)
		:ItemName(name), RefEnableName(enableName)
	{ }

	void RightMenuItemSeparator::Invoke(Engine::Object* uiPass)
	{
		ImGui::Separator();
	}

	RightMenuItemSubMenuConstant::RightMenuItemSubMenuConstant(const std::string& name)
		:RightMenuItemSubMenu(), RightMenuItemConstant(name)
	{ }

	void RightMenuItemSubMenuConstant::Invoke(Engine::Object* uiPass)
	{
		ASSERT(Next != nullptr);
		
		if (ImGui::BeginMenu(ItemName.c_str(), true))
		{
			ShowRightMenuContext(Next.get(), uiPass);
			ImGui::EndMenu();
		}
	}

	RightMenuItemSubMenuVariable::RightMenuItemSubMenuVariable(const std::string& name, const std::string& enableName)
		:RightMenuItemSubMenu(), RightMenuItemVariable(name, enableName)
	{ }

	void RightMenuItemSubMenuVariable::Invoke(Engine::Object* uiPass)
	{
		ASSERT(Next != nullptr);
		Engine::Class* class_ = Engine::Class::ForName(uiPass->GetType().mName);
		Engine::Field field = class_->GetField(RefEnableName);
		
		if (ImGui::BeginMenu(ItemName.c_str(), field.Get<bool>(uiPass)))
		{
			ShowRightMenuContext(Next.get(), uiPass);
			ImGui::EndMenu();
		}
	}

	RightMenuItemClick::RightMenuItemClick(const std::string& methodName)
		:RightMenuItemBase(), MethodName(methodName)
	{ }

	RightMenuItemClickConstant::RightMenuItemClickConstant(const std::string& name, const std::string& methodName)
		:RightMenuItemClick(methodName), RightMenuItemConstant(name)
	{ }

	void RightMenuItemClickConstant::Invoke(Engine::Object* uiPass)
	{
		if (ImGui::MenuItem(ItemName.c_str(), nullptr, false, true))
		{
			Engine::Class* class_ = Engine::Class::ForName(uiPass->GetType().mName);
			class_->GetMethod(MethodName).Call(uiPass);
		}
	}

	RightMenuItemClickVariable::RightMenuItemClickVariable(const std::string& name, const std::string& enableName, const std::string& methodName)
		:RightMenuItemClick(methodName), RightMenuItemVariable(name, enableName)
	{ }

	void RightMenuItemClickVariable::Invoke(Engine::Object* uiPass)
	{
		Engine::Class* class_ = Engine::Class::ForName(uiPass->GetType().mName);
		Engine::Field field = class_->GetField(RefEnableName);

		if (ImGui::MenuItem(ItemName.c_str(), nullptr, false, field.Get<bool>(uiPass)))
		{
			Engine::Class* class_ = Engine::Class::ForName(uiPass->GetType().mName);
			class_->GetMethod(MethodName).Call(uiPass);
		}
	}

}

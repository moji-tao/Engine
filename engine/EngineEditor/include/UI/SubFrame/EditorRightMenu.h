#pragma once
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include "EditorRightMenu.h"
#include "EngineRuntime/include/Core/Meta/Object.h"
#include "EngineRuntime/include/Core/Meta/Reflection.h"

namespace Editor
{
	class RightMenuItemBase;

	struct RightMenuContext
	{
		std::vector<std::shared_ptr<RightMenuItemBase>> List;
	};

	class RightMenuItemBase
	{
	public:
		RightMenuItemBase() = default;
		virtual ~RightMenuItemBase() = default;

	public:
		virtual void Invoke(Engine::Object* uiPass) = 0;
	};

	class RightMenuItemConstant
	{
	public:
		RightMenuItemConstant(const std::string& name);
		virtual ~RightMenuItemConstant() = default;

	public:
		std::string ItemName = "";
	};

	class RightMenuItemVariable
	{
	public:
		RightMenuItemVariable(const std::string& name, const std::string& enableName);
		virtual ~RightMenuItemVariable() = default;

	public:
		std::string ItemName = "";
		std::string RefEnableName = "";
	};

	class RightMenuItemSeparator : public RightMenuItemBase
	{
	public:
		RightMenuItemSeparator() = default;
		virtual ~RightMenuItemSeparator() override = default;

	public:
		virtual void Invoke(Engine::Object* uiPass) override;
	};

	class RightMenuItemSubMenu : public RightMenuItemBase
	{
	public:
		RightMenuItemSubMenu() = default;
		virtual ~RightMenuItemSubMenu() override = default;

	public:
		std::shared_ptr<RightMenuContext> Next = nullptr;
	};

	class RightMenuItemSubMenuConstant : public RightMenuItemSubMenu, public RightMenuItemConstant
	{
	public:
		RightMenuItemSubMenuConstant(const std::string& name);
		virtual ~RightMenuItemSubMenuConstant() override = default;

	public:
		virtual void Invoke(Engine::Object* uiPass) override;
	};

	class RightMenuItemSubMenuVariable : public RightMenuItemSubMenu, public RightMenuItemVariable
	{
	public:
		RightMenuItemSubMenuVariable(const std::string& name, const std::string& enableName);
		virtual ~RightMenuItemSubMenuVariable() override = default;

	public:
		virtual void Invoke(Engine::Object* uiPass) override;
	};

	class RightMenuItemClick : public RightMenuItemBase
	{
	public:
		RightMenuItemClick(const std::string& methodName);
		virtual ~RightMenuItemClick() override = default;

	protected:
		std::string MethodName = "";
	};

	class RightMenuItemClickConstant : public RightMenuItemClick, public RightMenuItemConstant
	{
	public:
		RightMenuItemClickConstant(const std::string& name, const std::string& methodName);
		virtual ~RightMenuItemClickConstant() override = default;

	public:
		virtual void Invoke(Engine::Object* uiPass) override;
	};

	class RightMenuItemClickVariable : public RightMenuItemClick, public RightMenuItemVariable
	{
	public:
		RightMenuItemClickVariable(const std::string& name, const std::string& enableName, const std::string& methodName);
		virtual ~RightMenuItemClickVariable() override = default;

	public:
		virtual void Invoke(Engine::Object* uiPass) override;
	};

	struct RightMenuUI
	{
		std::string MenuName = "";
		
		std::shared_ptr<RightMenuContext> Context = nullptr;
	};

	//std::shared_ptr<RightMenuUI> CreateRightMenu(const std::filesystem::path& filePath, const std::string& menuName);

	void ShowRightMenuContext(RightMenuContext* menu, Engine::Object* uiPass);

	class EditorRightMenu
	{
	public:
		static 	std::shared_ptr<RightMenuUI> CreateRightMenu(const std::filesystem::path& filePath, const std::string& menuName);

		static void DrawRightMenu(RightMenuUI* menu, Engine::Object* uiPass);

		static void ShowRightMenu(RightMenuUI* menu);

		static bool RightMenuIsPopup(RightMenuUI* menu);
	};
	
}

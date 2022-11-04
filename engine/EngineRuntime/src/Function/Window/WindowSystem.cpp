#include "EngineRuntime/include/Function/Window/WindowSystem.h"
#include "EngineRuntime/include/Core/Alloter/MemoryPool.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Platform/CharSet.h"

namespace Engine
{
	WindowSystem::~WindowSystem()
	{ }

	bool WindowSystem::Initialize(InitConfig* info)
	{
		if (!glfwInit())
		{
			LOG_FATAL("窗体初始化失败!");
			return false;
		}

		m_nWidth = info->WindowWidth;
		m_nHeight = info->WindowHeight;
		m_bFocusMode = false;

		size_t titleLength = strlen(info->Title) + 1;
		m_pTitle = (char*)GetBuffer(titleLength);
		memset(m_pTitle, 0x00, titleLength);
		memcpy(m_pTitle, info->Title, titleLength);

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_pWindow = glfwCreateWindow(m_nWidth, m_nHeight, m_pTitle, nullptr, nullptr);
		if (m_pWindow == nullptr)
		{
			LOG_FATAL("窗体创建失败!");
			glfwTerminate();
			return false;
		}

		glfwSetWindowUserPointer(m_pWindow, this);

		// 字符输入回调
		glfwSetCharCallback(m_pWindow, CharInputCallback);

		// 组合键回调，单个字符回调
		glfwSetCharModsCallback(m_pWindow, CharModsInputCallback);

		// 鼠标进入、离开窗体回调
		glfwSetCursorEnterCallback(m_pWindow, MouseEnterWindowCallback);

		// 鼠标移动时回调
		glfwSetCursorPosCallback(m_pWindow, MouseMoveCallback);

		// 窗口接受到文件放置时回调
		glfwSetDropCallback(m_pWindow, FilesDropCallback);

		// 错误回调
		//glfwSetErrorCallback(GLFWerrorfun);

		// 设置Framebuffer大小时回调
		glfwSetFramebufferSizeCallback(m_pWindow, FramebufferResizeCallback);

		// 连接、断开游戏杆时回调
		//glfwSetJoystickCallback(GLFWjoystickfun);

		// 按下按键或按键释放时回调
		glfwSetKeyCallback(m_pWindow, DownOrReleaseKeyCallback);

		// 设置显示器时回调
		//glfwSetMonitorCallback(GLFWmonitorfun);

		// 按下鼠标时回调
		glfwSetMouseButtonCallback(m_pWindow, MouseKeyInputCallback);

		// 改变窗口大小时回调
		glfwSetWindowSizeCallback(m_pWindow, WindowResizeCallback);

		// 
		//glfwSetWindowRefreshCallback(GLFWwindowrefreshfun);

		// 移动窗口时回调
		glfwSetWindowPosCallback(m_pWindow, WindowMoveCallback);

		// 窗口最大化或还原窗口时回调
		glfwSetWindowMaximizeCallback(m_pWindow, WindowMaximizeCallback);

		// 最小化时或恢复时回调
		glfwSetWindowIconifyCallback(m_pWindow, WindowIconifyCallback);

		// 窗口聚焦或失去聚焦时回调
		glfwSetWindowFocusCallback(m_pWindow, WindowFocusCallback);

		// 更改窗口比例时回调
		glfwSetWindowContentScaleCallback(m_pWindow, WindowContentScaleCallback);

		// 窗体关闭回调
		glfwSetWindowCloseCallback(m_pWindow, WindowCloseCallback);

		// 鼠标滚轮、触摸板手势回调
		glfwSetScrollCallback(m_pWindow, ScrollCallback);

		glfwSetWindowSizeLimits(m_pWindow, 100, 100, GLFW_DONT_CARE, GLFW_DONT_CARE);
		glfwSetInputMode(m_pWindow, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
		SetFocusMode(m_bFocusMode);

		return true;
	}

	bool WindowSystem::Tick(float deltaTile)
	{
		glfwPollEvents();
		return true;
	}

	void WindowSystem::Finalize()
	{
		if (m_pWindow != nullptr)
		{
			glfwDestroyWindow(m_pWindow);
			m_pWindow = nullptr;
			glfwTerminate();
		}

		if (m_pTitle != nullptr)
		{
			PushBuffer(m_pTitle);
			m_pTitle = nullptr;
		}
	}

	bool WindowSystem::ShouldClose() const
	{
		return glfwWindowShouldClose(m_pWindow);
	}

	const char* WindowSystem::GetTitle() const
	{
		return m_pTitle;
	}

	void WindowSystem::SetTitle(const char* title)
	{
		if (m_pTitle != nullptr)
		{
			PushBuffer(m_pTitle);
			m_pTitle = nullptr;
		}
		size_t uTitleLength = strlen(title) + 1;
		m_pTitle = (char*)GetBuffer(uTitleLength);
		memcpy(m_pTitle, title, uTitleLength);

		glfwSetWindowTitle(m_pWindow, m_pTitle);
	}

	void WindowSystem::SetTitle(const wchar_t* title)
	{
		if (m_pTitle != nullptr)
		{
			PushBuffer(m_pTitle);
			m_pTitle = nullptr;
		}
		size_t uTitleLength = UnicodeToAnsiLengthBuffer(title);
		m_pTitle = (char*)GetBuffer(uTitleLength);
		UnicodeToAnsi(title, m_pTitle);

		glfwSetWindowTitle(m_pWindow, m_pTitle);
	}


	int WindowSystem::GetWindowWidth() const
	{
		return m_nWidth;
	}

	int WindowSystem::GetWindowHeight() const
	{
		return m_nHeight;
	}

	std::pair<int, int> WindowSystem::GetWindowSize() const
	{
		return std::make_pair(m_nWidth, m_nHeight);
	}

	bool WindowSystem::GetFocusMode() const
	{
		return m_bFocusMode;
	}

	void WindowSystem::SetFocusMode(bool mode)
	{
		m_bFocusMode = mode;
		glfwSetInputMode(m_pWindow, GLFW_CURSOR, m_bFocusMode ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}

	HANDLE WindowSystem::GetWindowHandle() const
	{
#ifdef _WIN64
		return glfwGetWin32Window(m_pWindow);
#endif
		return m_pWindow;
	}

	void WindowSystem::SetFullScreenDisplay()
	{
		GLFWmonitor* primary = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primary);
		glfwSetWindowMonitor(m_pWindow, primary, 0, 0, mode->width, mode->height, mode->refreshRate);
	}

	void WindowSystem::SetWindowScreenDisplay(int width, int height)
	{
		GLFWmonitor* primary = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primary);

		width = min(width, mode->width);
		height = min(height, mode->height);

		glfwSetWindowMonitor(m_pWindow, nullptr, (mode->width - width) / 2, (mode->height - height) / 2, width, height, 0);
	}

	void WindowSystem::RegisterCharInputCallback(CharInputFunc func)
	{
		m_xOnCharInputFunc.push_back(func);
	}

	void WindowSystem::CharInputCallback(GLFWwindow* window, unsigned codepoint)
	{
		WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
		if (app != nullptr)
		{
			app->OnCharInput(codepoint);
		}
	}

	void WindowSystem::OnCharInput(unsigned codepoint)
	{
		for (auto& func : m_xOnCharInputFunc)
		{
			func(codepoint);
		}
	}

	void WindowSystem::RegisterCharModsInputCallback(CharModsInputFunc func)
	{
		m_xOnCharModsInputFunc.push_back(func);
	}

	void WindowSystem::CharModsInputCallback(GLFWwindow* window, unsigned codepoint, int mods)
	{
		WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
		if (app != nullptr)
		{
			app->OnCharModsInput(codepoint, mods);
		}
	}

	void WindowSystem::OnCharModsInput(unsigned codepoint, int mods)
	{
		for (auto& func : m_xOnCharModsInputFunc)
		{
			func(codepoint, mods);
		}
	}

	void WindowSystem::RegisterMouseEnterWindowCallback(MouseEnterWindowFunc func)
	{
		m_xOnMouseEnterWindowFunc.push_back(func);
	}

	void WindowSystem::MouseEnterWindowCallback(GLFWwindow* window, int edtered)
	{
		WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
		if (app != nullptr)
		{
			app->OnMouseEnterWindow(edtered);
		}
	}

	void WindowSystem::OnMouseEnterWindow(int edtered)
	{
		for (auto& func : m_xOnMouseEnterWindowFunc)
		{
			func(edtered);
		}
	}

	void WindowSystem::RegisterMouseMoveCallback(MouseMoveFunc func)
	{
		m_xOnMouseMoveFunc.push_back(func);
	}

	void WindowSystem::MouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
	{
		WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
		if (app != nullptr)
		{
			app->OnMouseMove(xpos, ypos);
		}
	}

	void WindowSystem::OnMouseMove(double xpos, double ypos)
	{
		for (auto& func : m_xOnMouseMoveFunc)
		{
			func(xpos, ypos);
		}
	}

	void WindowSystem::RegisterFilesDropCallback(FilesDropFunc func)
	{
		m_xOnFilesDropFunc.push_back(func);
	}

	void WindowSystem::FilesDropCallback(GLFWwindow* window, int path_count, const char** path)
	{
		WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
		if (app != nullptr)
		{
			app->OnFilesDrop(path_count, path);
		}
	}

	void WindowSystem::OnFilesDrop(int path_count, const char** path)
	{
		for (auto& func : m_xOnFilesDropFunc)
		{
			func(path_count, path);
		}
	}

	void WindowSystem::RegisterFramebufferResizeCallback(FramebufferResizeFunc func)
	{
		m_xOnFramebufferResizeFunc.push_back(func);
	}

	void WindowSystem::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
		if (app != nullptr)
		{
			app->OnFramebufferResize(width, height);
		}
	}

	void WindowSystem::OnFramebufferResize(int width, int height)
	{
		for (auto& func : m_xOnFramebufferResizeFunc)
		{
			func(width, height);
		}
	}

	void WindowSystem::RegisterDownOrReleaseKeyCallback(DownOrReleaseKeyFunc func)
	{
		m_xOnDownOrReleaseKeyFunc.push_back(func);
	}

	void WindowSystem::DownOrReleaseKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
		if (app != nullptr)
		{
			app->OnDownOrReleaseKey(key, scancode, action, mods);
		}
	}

	void WindowSystem::OnDownOrReleaseKey(int key, int scancode, int action, int mods)
	{
		for (auto& func : m_xOnDownOrReleaseKeyFunc)
		{
			func(key, scancode, action, mods);
		}
	}

	void WindowSystem::RegisterMouseKeyInputCallback(MouseKeyInputFunc func)
	{
		m_xOnMouseKeyInputFunc.push_back(func);
	}

	void WindowSystem::MouseKeyInputCallback(GLFWwindow* window, int button, int action, int mods)
	{
		WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
		if (app != nullptr)
		{
			app->OnMouseKeyInput(button, action, mods);
		}
	}

	void WindowSystem::OnMouseKeyInput(int button, int action, int mods)
	{
		for (auto& func : m_xOnMouseKeyInputFunc)
		{
			func(button, action, mods);
		}
	}

	void WindowSystem::RegisterWindowResizeCallback(WindowResizeFunc func)
	{
		m_xOnWindowResizeFunc.push_back(func);
	}

	void WindowSystem::WindowResizeCallback(GLFWwindow* window, int width, int height)
	{
		WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);

		app->m_nWidth = width;
		app->m_nHeight = height;

		if (app != nullptr)
		{
			app->OnWindowResize(width, height);
		}
	}

	void WindowSystem::OnWindowResize(int width, int height)
	{
		for (auto& func : m_xOnWindowResizeFunc)
		{
			func(width, height);
		}
	}

	void WindowSystem::RegisterWindowMoveCallback(WindowMoveFunc func)
	{
		m_xOnWindowMoveFunc.push_back(func);
	}

	void WindowSystem::WindowMoveCallback(GLFWwindow* window, int xpos, int ypos)
	{
		WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
		if (app != nullptr)
		{
			app->OnWindowMove(xpos, ypos);
		}
	}

	void WindowSystem::OnWindowMove(int xpos, int ypos)
	{
		for (auto& func : m_xOnWindowMoveFunc)
		{
			func(xpos, ypos);
		}
	}

	void WindowSystem::RegisterWindowMaximizeCallback(WindowMaximizeFunc func)
	{
		m_xOnWindowMaximizeFunc.push_back(func);
	}

	void WindowSystem::WindowMaximizeCallback(GLFWwindow* window, int maximized)
	{
		WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
		if (app != nullptr)
		{
			app->OnWindowMaximize(maximized);
		}
	}

	void WindowSystem::OnWindowMaximize(int maximized)
	{
		for (auto& func : m_xOnWindowMaximizeFunc)
		{
			func(maximized);
		}
	}

	void WindowSystem::RegisterWindowIconifyCallback(WindowIconifyFunc func)
	{
		m_xOnWindowIconifyFunc.push_back(func);
	}

	void WindowSystem::WindowIconifyCallback(GLFWwindow* window, int iconified)
	{
		WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
		if (app != nullptr)
		{
			app->OnWindowIconify(iconified);
		}
	}

	void WindowSystem::OnWindowIconify(int iconified)
	{
		for (auto& func : m_xOnWindowIconifyFunc)
		{
			func(iconified);
		}
	}

	void WindowSystem::RegisterWindowFocusCallback(WindowFocusFunc func)
	{
		m_xOnWindowFocusFunc.push_back(func);
	}

	void WindowSystem::WindowFocusCallback(GLFWwindow* window, int focused)
	{
		WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
		if (app != nullptr)
		{
			app->OnWindowFocus(focused);
		}
	}

	void WindowSystem::OnWindowFocus(int focused)
	{
		for (auto& func : m_xOnWindowFocusFunc)
		{
			func(focused);
		}
	}

	void WindowSystem::RegisterWindowContentScaleCallback(WindowContentScaleFunc func)
	{
		m_xOnWindowContentScaleFunc.push_back(func);
	}

	void WindowSystem::WindowContentScaleCallback(GLFWwindow* window, float xscale, float yscale)
	{
		WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
		if (app != nullptr)
		{
			app->OnWindowContentScale(xscale, yscale);
		}
	}

	void WindowSystem::OnWindowContentScale(float xscale, float yscale)
	{
		for (auto& func : m_xOnWindowContentScaleFunc)
		{
			func(xscale, yscale);
		}
	}

	void WindowSystem::RegisterWindowCloseCallback(WindowCloseFunc func)
	{
		m_xOnWindowCloseFunc.push_back(func);
	}

	void WindowSystem::WindowCloseCallback(GLFWwindow* window)
	{
		WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);

		if (app != nullptr)
		{
			if (app->m_pTitle != nullptr)
			{
				LOG_INFO("{0} 窗口已关闭", app->m_pTitle);
				PushBuffer(app->m_pTitle);
				app->m_pTitle = nullptr;
			}
			app->OnWindowClose();
		}
	}

	void WindowSystem::OnWindowClose()
	{
		for (auto& func : m_xOnWindowCloseFunc)
		{
			func();
		}
	}

	void WindowSystem::RegisterScrollCallback(ScrollFunc func)
	{
		m_xOnScrollFunc.push_back(func);
	}

	void WindowSystem::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
		if (app != nullptr)
		{
			app->OnScroll(xoffset, yoffset);
		}
	}

	void WindowSystem::OnScroll(double xoffset, double yoffset)
	{
		for (auto& func : m_xOnScrollFunc)
		{
			func(xoffset, yoffset);
		}
	}
}


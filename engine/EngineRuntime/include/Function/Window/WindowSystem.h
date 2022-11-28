#pragma once
#include <functional>
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"
#include "EngineRuntime/include/Framework/Interface/IRuntimeModule.h"
#include "GLFW/glfw3.h"
#ifdef _WIN64
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#endif

namespace Engine
{
	class WindowSystem : public IRuntimeModule, public ISingleton<WindowSystem>
	{
	public:
		WindowSystem() = default;
		virtual ~WindowSystem() override;

		typedef void* HANDLE;

	public:
		virtual bool Initialize(InitConfig* info) override;

		virtual bool Tick(float deltaTile) override;

		virtual void Finalize() override;

	public:
		bool ShouldClose() const;

		const char* GetTitle() const;

		void SetTitle(const char* title);

		void SetTitle(const wchar_t* title);

		void SetTitle(const std::string title);

		int GetWindowWidth() const;

		int GetWindowHeight() const;

		std::pair<int, int> GetWindowSize() const;

		bool GetFocusMode() const;

		void SetFocusMode(bool mode);

		HANDLE GetWindowHandle(bool isNative) const;

		void SetFullScreenDisplay();

		void SetWindowScreenDisplay(int width, int height);

		void CloseWindow();

	private:
		GLFWwindow* m_pWindow = nullptr;
		int m_nWidth;
		int m_nHeight;
		bool m_bFocusMode = false;
		//char* m_pTitle = nullptr;
		std::string m_pTitle;

		std::string m_defuleTitle;

	public:
		typedef std::function<void(unsigned)> CharInputFunc;
		typedef std::function<void(unsigned, int)> CharModsInputFunc;
		typedef std::function<void(int)> MouseEnterWindowFunc;
		typedef std::function<void(double, double)> MouseMoveFunc;
		typedef std::function<void(int, const char**)> FilesDropFunc;
		typedef std::function<void(int, int)> FramebufferResizeFunc;
		typedef std::function<void(int, int, int, int)> DownOrReleaseKeyFunc;
		typedef std::function<void(int, int, int)> MouseKeyInputFunc;
		typedef std::function<void(int, int)> WindowResizeFunc;
		typedef std::function<void(int, int)> WindowMoveFunc;
		typedef std::function<void(int)> WindowMaximizeFunc;
		typedef std::function<void(int)> WindowIconifyFunc;
		typedef std::function<void(int)> WindowFocusFunc;
		typedef std::function<void(float, float)> WindowContentScaleFunc;
		typedef std::function<void()> WindowCloseFunc;
		typedef std::function<void(double, double)> ScrollFunc;

	public:
		/**
		 * \brief 注册字符输入回调
		 * \param func void(unsigned)
		 */
		void RegisterCharInputCallback(CharInputFunc func);

		/**
		 * \brief 组合键回调，单个字符回调
		 * \param func void(unsigned, int)
		 */
		void RegisterCharModsInputCallback(CharModsInputFunc func);

		/**
		 * \brief 鼠标进入、离开窗体回调
		 * \param func void(int)
		 */
		void RegisterMouseEnterWindowCallback(MouseEnterWindowFunc func);

		/**
		 * \brief 鼠标移动时回调
		 * \param func void(double, double)
		 */
		void RegisterMouseMoveCallback(MouseMoveFunc func);

		/**
		 * \brief 窗口接受到文件放置时回调
		 * \param func void(int, const char**)
		 */
		void RegisterFilesDropCallback(FilesDropFunc func);

		/**
		 * \brief 设置Framebuffer大小时回调
		 * \param func void(int, int)
		 */
		void RegisterFramebufferResizeCallback(FramebufferResizeFunc func);

		/**
		 * \brief 按下按键或按键释放时回调
		 * \param func void(int, int, int, int)
		 */
		void RegisterDownOrReleaseKeyCallback(DownOrReleaseKeyFunc func);

		/**
		 * \brief 按下鼠标时回调
		 * \param func void(int, int, int)
		 */
		void RegisterMouseKeyInputCallback(MouseKeyInputFunc func);

		/**
		 * \brief 改变窗口大小时回调
		 * \param func void(int, int)
		 */
		void RegisterWindowResizeCallback(WindowResizeFunc func);

		/**
		 * \brief 移动窗口时回调
		 * \param func void(int, int)
		 */
		void RegisterWindowMoveCallback(WindowMoveFunc func);

		/**
		 * \brief 窗口最大化或还原窗口时回调
		 * \param func void(int)
		 */
		void RegisterWindowMaximizeCallback(WindowMaximizeFunc func);

		/**
		 * \brief 最小化时或恢复时回调
		 * \param func void(int)
		 */
		void RegisterWindowIconifyCallback(WindowIconifyFunc func);

		/**
		 * \brief 窗口聚焦或失去聚焦时回调
		 * \param func void(int)
		 */
		void RegisterWindowFocusCallback(WindowFocusFunc func);

		/**
		 * \brief 更改窗口比例时回调
		 * \param func void(float, float)
		 */
		void RegisterWindowContentScaleCallback(WindowContentScaleFunc func);

		/**
		 * \brief 窗体关闭回调
		 * \param func void()
		 */
		void RegisterWindowCloseCallback(WindowCloseFunc func);

		/**
		 * \brief 鼠标滚轮、触摸板手势回调
		 * \param func void(double, double)
		 */
		void RegisterScrollCallback(ScrollFunc func);

	protected:
		/**
		 * \brief 字符输入回调
		 */
		static void CharInputCallback(GLFWwindow* window, unsigned codepoint);

		void OnCharInput(unsigned codepoint);

		/**
		 * \brief 组合键回调，单个字符回调
		 */
		static void CharModsInputCallback(GLFWwindow* window, unsigned codepoint, int mods);

		void OnCharModsInput(unsigned codepoint, int mods);

		/**
		 * \brief 鼠标进入、离开窗体回调
		 */
		static void MouseEnterWindowCallback(GLFWwindow* window, int edtered);

		void OnMouseEnterWindow(int edtered);

		/**
		 * \brief 鼠标移动时回调
		 */
		static void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);

		void OnMouseMove(double xpos, double ypos);

		/**
		 * \brief 窗口接受到文件放置时回调
		 */
		static void FilesDropCallback(GLFWwindow* window, int path_count, const char** path);

		void OnFilesDrop(int path_count, const char** path);

		/**
		 * \brief 设置Framebuffer大小时回调
		 */
		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

		void OnFramebufferResize(int width, int height);

		/**
		 * \brief 按下按键或按键释放时回调
		 */
		static void DownOrReleaseKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

		void OnDownOrReleaseKey(int key, int scancode, int action, int mods);

		/**
		 * \brief 按下鼠标时回调
		 */
		static void MouseKeyInputCallback(GLFWwindow* window, int button, int action, int mods);

		void OnMouseKeyInput(int button, int action, int mods);

		/**
		 * \brief 改变窗口大小时回调
		 */
		static void WindowResizeCallback(GLFWwindow* window, int width, int height);

		void OnWindowResize(int width, int height);

		/**
		 * \brief 移动窗口时回调
		 */
		static void WindowMoveCallback(GLFWwindow* window, int xpos, int ypos);

		void OnWindowMove(int xpos, int ypos);

		/**
		 * \brief 窗口最大化或还原窗口时回调
		 */
		static void WindowMaximizeCallback(GLFWwindow* window, int maximized);

		void OnWindowMaximize(int maximized);

		/**
		 * \brief 最小化时或恢复时回调
		 */
		static void WindowIconifyCallback(GLFWwindow* window, int iconified);

		void OnWindowIconify(int iconified);

		/**
		 * \brief 窗口聚焦或失去聚焦时回调
		 */
		static void WindowFocusCallback(GLFWwindow* window, int focused);

		void OnWindowFocus(int focused);

		/**
		 * \brief 更改窗口比例时回调
		 */
		static void WindowContentScaleCallback(GLFWwindow* window, float xscale, float yscale);

		void OnWindowContentScale(float xscale, float yscale);

		/**
		 * \brief 窗体关闭回调
		 */
		static void WindowCloseCallback(GLFWwindow* window);

		void OnWindowClose();

		/**
		 * \brief 鼠标滚轮、触摸板手势回调
		 */
		static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

		void OnScroll(double xoffset, double yoffset);

	private:
		std::vector<CharInputFunc> m_xOnCharInputFunc;
		std::vector<CharModsInputFunc> m_xOnCharModsInputFunc;
		std::vector<MouseEnterWindowFunc> m_xOnMouseEnterWindowFunc;
		std::vector<MouseMoveFunc> m_xOnMouseMoveFunc;
		std::vector<FilesDropFunc> m_xOnFilesDropFunc;
		std::vector<FramebufferResizeFunc> m_xOnFramebufferResizeFunc;
		std::vector<DownOrReleaseKeyFunc> m_xOnDownOrReleaseKeyFunc;
		std::vector<MouseKeyInputFunc> m_xOnMouseKeyInputFunc;
		std::vector<WindowResizeFunc> m_xOnWindowResizeFunc;
		std::vector<WindowMoveFunc> m_xOnWindowMoveFunc;
		std::vector<WindowMaximizeFunc> m_xOnWindowMaximizeFunc;
		std::vector<WindowIconifyFunc> m_xOnWindowIconifyFunc;
		std::vector<WindowFocusFunc> m_xOnWindowFocusFunc;
		std::vector<WindowContentScaleFunc> m_xOnWindowContentScaleFunc;
		std::vector<WindowCloseFunc> m_xOnWindowCloseFunc;
		std::vector<ScrollFunc> m_xOnScrollFunc;
	};

}


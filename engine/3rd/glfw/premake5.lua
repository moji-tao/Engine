project "glfw"
    location (g_WorkSpaceRootFolder .. "build/3rd/%{prj.name}/")

	kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    targetdir (g_WorkSpaceRootFolder .. "lib/")	-- 指定输出目录
    objdir (g_WorkSpaceRootFolder .. "bin-int/3rd/%{prj.name}/")		-- 指定中间目录

	files
	{
        "include/GLFW/glfw3.h",
		"include/GLFW/glfw3native.h",
		"src/internal.h",
		"src/platform.h",
		"src/mappings.h",
		"src/null_platform.h",
		"src/null_joystick.h",
		"src/context.c",
		"src/init.c",
		"src/input.c",
		"src/monitor.c",
		"src/platform.c",
		"src/vulkan.c",
		"src/window.c",
		"src/egl_context.c",
		"src/osmesa_context.c",
		"src/null_init.c",
		"src/null_monitor.c",
		"src/null_window.c",
		"src/null_joystick.c"
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		files
		{
			"src/win32_time.h",
			"src/win32_thread.h",
			"src/win32_platform.h",
			"src/win32_joystick.h",
			"src/win32_module.c",
			"src/win32_time.c",
			"src/win32_thread.c",
			"src/win32_init.c",
			"src/win32_joystick.c",
			"src/win32_monitor.c",
			"src/win32_window.c",
			"src/wgl_context.c",
		}

		defines 
		{ 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS",
		}
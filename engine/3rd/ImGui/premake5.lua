project "ImGui"
    location (g_WorkSpaceRootFolder .. "build/3rd/%{prj.name}/")

	kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    targetdir (g_WorkSpaceRootFolder .. "lib/")	-- 指定输出目录
    objdir (g_WorkSpaceRootFolder .. "bin-int/3rd/%{prj.name}/")		-- 指定中间目录

	files
	{
        "include/**.h",
        "src/imgui.cpp",
        "src/imgui_demo.cpp",
        "src/imgui_draw.cpp",
        "src/imgui_tables.cpp",
        "src/imgui_widgets.cpp",
        "src/backends/imgui_impl_dx12.cpp",
        "src/backends/imgui_impl_glfw.cpp",
        "src/backends/imgui_impl_win32.cpp"
	}

    includedirs
    {
        "%{IncludeDir.glfw}",
        "%{IncludeDir.imgui}",
		"%{IncludeDir.imgui}/imgui/backends/"
    }
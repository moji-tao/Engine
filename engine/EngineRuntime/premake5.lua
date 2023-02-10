project "EngineRuntime"
    location (g_WorkSpaceRootFolder .. "build/%{prj.name}/")

    kind "StaticLib"

    language "C++"
    cppdialect "C++20"

    targetdir (g_WorkSpaceRootFolder .. "lib/")	-- 指定输出目录
    objdir (g_WorkSpaceRootFolder .. "bin-int/%{prj.name}/")		-- 指定中间目录

    libdirs (g_WorkSpaceRootFolder .. "lib/")

    files 
    { 
        g_WorkSpaceRootFolder .. "engine/%{prj.name}/include/**.h",
        g_WorkSpaceRootFolder .. "engine/%{prj.name}/src/**.cpp"
    }

    links
    {
        "glfw",
        "ImGui",
        "jsoncpp",
        "spdlog",
        "jsoncpp",
        "yamlcpp"
    }

    includedirs
    {
        "%{IncludeDir.glfw}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.assimp}",
        "%{IncludeDir.Boost}",
        "%{IncludeDir.jsoncpp}",
        "%{IncludeDir.yamlcpp}",
        "%{IncludeDir.spdlog}",
        g_WorkSpaceRootFolder .. "engine/"
    }

    defines
    {
        "NOMINMAX",
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "system:windows"
        buildoptions 
        {
            "/permissive"
        }
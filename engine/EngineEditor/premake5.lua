project "EngineEditor"
    location (g_WorkSpaceRootFolder .. "build/%{prj.name}/")

    kind "ConsoleApp"

    language "C++"
    cppdialect "C++20"

    targetdir (g_WorkSpaceRootFolder .. "bin/")	-- 指定输出目录
    objdir (g_WorkSpaceRootFolder .. "bin-int/%{prj.name}/")		-- 指定中间目录

    libdirs (g_WorkSpaceRootFolder .. "lib/")

    files 
    { 
        g_WorkSpaceRootFolder .. "engine/%{prj.name}/include/**.h",
        g_WorkSpaceRootFolder .. "engine/%{prj.name}/src/**.cpp"
    }

    links
    {
        "EngineRuntime",
        "assimp-mt.lib",
        "d3d12.lib",
        "d3dcompiler.lib",
        "dxgi.lib",
        "dxguid.lib"
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
        "%{IncludeDir.Lua}",
        "%{IncludeDir.sol2}",
        g_WorkSpaceRootFolder .. "engine/"
    }

    defines
    {
        "NOMINMAX"
    }

    prebuildcommands
	{
		("{COPYFILE} " .. g_WorkSpaceRootFolder .. "engine/3rd/assimp/lib/assimp-mt.lib " .. g_WorkSpaceRootFolder .. "lib/")
	}

	postbuildcommands
	{
        ("{COPYDIR} " .. g_WorkSpaceRootFolder .. "engine/bin/ " .. g_WorkSpaceRootFolder .. "./bin"),
		("{COPYFILE} " .. g_WorkSpaceRootFolder .. "engine/3rd/assimp/bin/assimp-mt.dll " .. g_WorkSpaceRootFolder .. "bin/")
	}

    debugdir "$(SolutionDir)bin/"

    filter "configurations:Debug"
        debugargs
        {
            "D:/WorkSpace/Test"
        }
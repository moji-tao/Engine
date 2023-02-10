project "HLSLCompiler"
    location (g_WorkSpaceRootFolder .. "build/%{prj.name}/")

    kind "ConsoleApp"

    language "C++"
    cppdialect "C++20"

    targetdir (g_WorkSpaceRootFolder .. "bin/")	-- 指定输出目录
    objdir (g_WorkSpaceRootFolder .. "bin-int/%{prj.name}/")		-- 指定中间目录

    libdirs (g_WorkSpaceRootFolder .. "lib/")

    files 
    { 
        g_WorkSpaceRootFolder .. "engine/%{prj.name}/**.h",
        g_WorkSpaceRootFolder .. "engine/%{prj.name}/**.hpp",
        g_WorkSpaceRootFolder .. "engine/%{prj.name}/**.cpp"
    }

    links
    {
        "d3d12.lib",
        "d3dcompiler.lib",
        "dxgi.lib",
        "dxguid.lib"
    }

    includedirs
    {
        "%{IncludeDir.jsoncpp}",
        g_WorkSpaceRootFolder .. "engine/"
    }

    links
    {
        "jsoncpp",
    }


project "Lua"
    location (g_WorkSpaceRootFolder .. "build/3rd/%{prj.name}/")

    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    targetdir (g_WorkSpaceRootFolder .. "lib/")	-- 指定输出目录
    objdir (g_WorkSpaceRootFolder .. "bin-int/3rd/%{prj.name}/")		-- 指定中间目录

    files
    {
        "src/**.h",
        "src/**.c"
    }

    includedirs
    {
        "%{IncludeDir.Lua}",
    }

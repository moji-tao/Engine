g_WorkSpaceRootFolder = (os.getcwd() .. "/../../")
print("在路径: " .. g_WorkSpaceRootFolder .. " 下生成解决方案")

workspace "GameEngine"
    configurations { "Debug", "Release" }

    location (g_WorkSpaceRootFolder)

    platforms { "x64" }

    filter "system:windows"
        systemversion "latest"

    filter "platforms:x64"
        architecture "x64"

    -- Debug配置项属性
	filter "configurations:Debug"
    defines { "DEBUG" }					    -- 定义DEBUG宏（这可以算是默认配置）
        symbols "On"						-- 开启调试符号，

    -- Release配置项属性
    filter "configurations:Release"
        defines { "NDEBUG" }				-- 定义NDEBUG宏（这可以算是默认配置）
        optimize "On"						-- 开启优化参数

    include(g_WorkSpaceRootFolder .. "engine/3rd/premake5.lua")
    include(g_WorkSpaceRootFolder .. "engine/EngineEditor/premake5.lua")
    include(g_WorkSpaceRootFolder .. "engine/EngineRuntime/premake5.lua")
    include(g_WorkSpaceRootFolder .. "engine/HLSLCompiler/premake5.lua")
    
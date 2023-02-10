IncludeDir = {}
IncludeDir["glfw"] = g_WorkSpaceRootFolder .. "engine/3rd/glfw/include"
IncludeDir["imgui"] = g_WorkSpaceRootFolder .. "engine/3rd/ImGui/include"
IncludeDir["stb"] = g_WorkSpaceRootFolder .. "engine/3rd/stb/include"
IncludeDir["assimp"] = g_WorkSpaceRootFolder .. "engine/3rd/assimp/include"
IncludeDir["Boost"] = g_WorkSpaceRootFolder .. "engine/3rd/Boost"
IncludeDir["jsoncpp"] = g_WorkSpaceRootFolder .. "engine/3rd/jsoncpp/include"
IncludeDir["yamlcpp"] = g_WorkSpaceRootFolder .. "engine/3rd/yamlcpp/include"
IncludeDir["spdlog"] = g_WorkSpaceRootFolder .. "engine/3rd/spdlog/include"

include(g_WorkSpaceRootFolder .. "engine/3rd/glfw/premake5.lua")
include(g_WorkSpaceRootFolder .. "engine/3rd/ImGui/premake5.lua")
include(g_WorkSpaceRootFolder .. "engine/3rd/spdlog/premake5.lua")
include(g_WorkSpaceRootFolder .. "engine/3rd/jsoncpp/premake5.lua")
include(g_WorkSpaceRootFolder .. "engine/3rd/yamlcpp/premake5.lua")


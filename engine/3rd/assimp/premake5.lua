project "assimp"
	location (g_WorkSpaceRootFolder .. "build/3rd/%{prj.name}/")

	kind "SharedLib"

	language "C++"

	targetdir (g_WorkSpaceRootFolder .. "bin")	-- 指定输出目录
	objdir (g_WorkSpaceRootFolder .. "bin-int/3rd/%{prj.name}/")		-- 指定中间目录

	
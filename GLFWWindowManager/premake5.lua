include "../premakeDefines.lua"

project "GLFWWindowManager"
	kind "StaticLib"
	language "C++"
	staticruntime "off"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../obj/" .. outputdir .. "/%{prj.name}")
	
	pchheader "GLFWWindowManagerStdafx.h"
	pchsource "GLFWWindowManagerStdafx.cpp"
	
	Link("Utils")
	Link("Core")

	files
	{
		"GLFWWindowManager/**.h",
		"GLFWWindowManager/**.cpp",
		"GLFWWindowManager/**.inl",
		"GLFWWindowManagerStdafx.cpp",
		"GLFWWindowManagerStdafx.h",
		"premake5.lua",
	}

	includedirs
	{
		".",
		"../Renderer",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
	}

	links 
	{ 
		"GLFW",
		"pugixml",
		"ImGui",
	}

	defines
	{
		"IMGUI_IMPL_OPENGL_LOADER_GLAD"
	}

	filter "system:windows"
		defines
		{
			"BUILD_GLFWWINDOWMANAGER_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\""),
		}

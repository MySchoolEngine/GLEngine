include "../premakeDefines.lua"

project "GLRenderer"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../obj/" .. outputdir .. "/%{prj.name}")
	
	pchheader "GLRendererStdafx.h"
	pchsource "GLRendererStdafx.cpp"
	
	Link("Entity")
	Link("Utils")
	Link("Renderer")
	Link("GLFWWindowManager")
	Link("Core")
	Link("GUI")
	Link("ImGuiFileDialog")

	files
	{
		"GLRenderer/**.h",
		"GLRenderer/**.cpp",
		"GLRenderer/**.inl",
		"GLRendererStdafx.cpp",
		"GLRendererStdafx.h",
		"premake5.lua",
	}

	includedirs
	{
		".",
		"../Physics",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.Glad}",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.GLI}",
		"../%{IncludeDir.pugixml}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.ImGui}",
		"../%{IncludeDir.ImGuiFileDialog}",
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"opengl32.lib",
		"pugixml",
		"ImGui",
		"DevIL-IL",
	}

	-- used in ImGui\examples\imgui_impl_opengl3.cpp
	defines
	{
		"IMGUI_IMPL_OPENGL_LOADER_GLAD",
		"IMGUI_API=__declspec(dllimport)",
		"IMGUI_IMPL_API=",
	}

	filter "system:windows"
		defines
		{
			"BUILD_GLRENDERER_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\""),
		}

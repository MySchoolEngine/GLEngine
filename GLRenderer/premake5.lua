include "../premakeDefines.lua"

project "GLRenderer"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	SetupProject("GLRenderer")
	
	PrecompiledHeaders("GLRenderer")

	Link("Entity")
	Link("Utils")
	Link("Renderer")
	Link("Core")
	Link("GUI")
	Link("Audio")

	includedirs
	{
		"../Physics",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.Glad}",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.GLI}",
		"../%{IncludeDir.pugixml}",
		"../%{IncludeDir.ImGui}",
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"pugixml",
		"ImGui",
		"DevIL-IL",
	}

	-- used in ImGui\examples\imgui_impl_opengl3.cpp
	defines
	{
		"IMGUI_IMPL_OPENGL_LOADER_GLAD",
		"IMGUI_IMPL_API=",
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"BUILD_GLRENDERER_DLL",
			"IMGUI_API=__declspec(dllimport)",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\""),
		}

		links
		{
			"opengl32.lib",
		}

	filter "system:linux"
		pic "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

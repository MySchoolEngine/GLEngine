include "../Tools/Premake5/premakeDefines.lua"

project "GLRenderer"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	SetupProject("GLRenderer")
	PrecompiledHeaders("GLRenderer")

	Link("Entity")
	Link("Utils")
	Link("Renderer")
	Link("GLFWWindowManager")
	Link("Core")
	Link("GUI")
	Link("Editor")
--	Link("Animation") -- until the whole skeletal Animation could be moved outside of API specific implementation

	LinkDependency("ImGui")
	LinkDependency("pugixml")
	LinkDependency("GLFW")
	LinkDependency("RTTR")

	includedirs
	{
		"../Physics",
		"../%{IncludeDir.Glad}",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.GLI}",
		"../%{IncludeDir.fmt}",
		"%{wks.location}/%{IncludeDir.slot_map}",
	}

	links 
	{ 
		"Glad",
		"DevIL-IL",
	}

	-- used in ImGui\examples\imgui_impl_opengl3.cpp
	defines
	{
		"IMGUI_IMPL_OPENGL_LOADER_GLAD",
		"IMGUI_IMPL_API=",
	}

	filter "system:windows"
		defines
		{
			"IMGUI_API=__declspec(dllimport)",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\""),
		}

		links
		{
			"opengl32.lib",
		}

	filter "system:linux"
		pic "On"
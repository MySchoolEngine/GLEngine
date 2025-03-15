include "../Tools/Premake5/premakeDefines.lua"

project "AI"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	SetupProject("AI")
	PrecompiledHeaders("AI")

	Link("Entity")
	Link("Utils")
	Link("Renderer")

	LinkDependency("ImGui")
	LinkDependency("pugixml")
	LinkDependency("RTTR")

	includedirs
	{
		"../Physics",
		"../%{IncludeDir.GLM}",
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

	filter "system:linux"
		pic "On"
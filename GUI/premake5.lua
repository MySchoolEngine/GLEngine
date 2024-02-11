include "../Tools/Premake5/premakeDefines.lua"

project "GUI"
	kind "SharedLib"
	language "C++"

	SetupProject("GUI")
	PrecompiledHeaders("GUI")

	Link("Core")
	Link("Utils")
	Link("ImGuiFileDialog")

	LinkDependency("ImGui")
	LinkDependency("RTTR")

	includedirs
	{
		"../Renderer",
		"../Physics",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.ImGuiFileDialog}",
		"%{wks.location}/%{IncludeDir.slot_map}",
	}

	filter "system:windows"
		defines
		{
			"IMGUI_API=__declspec(dllimport)"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\""),
		}

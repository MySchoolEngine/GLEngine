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

	includedirs
	{
		"../Renderer",
		"../Physics",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.ImGuiFileDialog}",
	}

	filter "system:windows"
		defines
		{
			"IMGUI_API=__declspec(dllimport)"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\""),
		}

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
		"%{wks.location}/%{IncludeDir.IconFontCppHeaders}",
	}

	filter "system:windows"
		defines
		{
			"IMGUI_API=__declspec(dllimport)"
		}

		CopyToSandbox()
	filter {}

	usage "PUBLIC"
		uses{"Core"}

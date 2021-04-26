include "../premakeDefines.lua"

project "GUI"
	kind "SharedLib"
	language "C++"

	SetupProject("GUI")
	PrecompiledHeaders("GUI")

	Link("Core")
	Link("Utils")
	Link("ImGuiFileDialog")

	includedirs
	{
		"../Renderer",
		"../Physics",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.ImGui}",
		"../%{IncludeDir.ImGuiFileDialog}",
	}

	links 
	{ 
		"ImGui",
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

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

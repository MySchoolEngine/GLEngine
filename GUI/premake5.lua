include "../premakeDefines.lua"

project "GUI"
	kind "SharedLib"
	language "C++"

	SetupProject("GUI")
	PrecompiledHeaders("GUI")

	Link("Core")
	Link("Utils")

	includedirs
	{
		"../Renderer",
		"../Physics",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.ImGui}",
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

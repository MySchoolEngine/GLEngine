include "../premakeDefines.lua"

project "GUI"
	kind "StaticLib"
	language "C++"

	SetupProject("GUI")
	PrecompiledHeaders("GUI")
	
	Link("Core")
	Link("Utils")
	Link("Entity")

	includedirs
	{
		"../Renderer",
		"../Core",
		"../Physics",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.ImGui}",
	}

	links 
	{ 
		"ImGui",
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"BUILD_GUI_DLL",
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

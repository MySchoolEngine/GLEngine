include "../premakeDefines.lua"

project "Sandbox"
    kind "ConsoleApp"
	language "C++"
	staticruntime "off"
	
	debugdir ("../data/")
	SetupProject("Sandbox")
	debugdir ("../data/")
	
	pchheader "SandboxStdafx.h"
	pchsource "SandboxStdafx.cpp"
	
	Link("Core")
	Link("Utils")
	Link("GLRenderer")

	files
	{
		"SandboxStdafx.cpp",
		"SandboxStdafx.h",
	}

	includedirs
	{
		"."
		"../Renderer",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
	}

	filter "system:windows"
		kind "WindowedApp"
		cppdialect "C++17"
		systemversion "latest"
		Link("DX12Renderer")

	filter "system:linux"
		links
		{
			"X11",
		}
		linkoptions "-pthread"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

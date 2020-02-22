include "../premakeDefines.lua"

project "Sandbox"
    kind "ConsoleApp"
	language "C++"
	staticruntime "off"
	
	debugdir ("../data/")
	SetupProject("Sandbox")
	debugdir ("../data/")
	
	
	Link("Core")
	Link("Utils")
	Link("GLRenderer")
	Link("DX12Renderer")

	includedirs
	{
		"../Renderer",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
	}

	filter "system:windows"
    	kind "WindowedApp"
		cppdialect "C++17"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

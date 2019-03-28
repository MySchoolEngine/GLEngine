include "../premakeDefines.lua"

project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"
	
	SetupProject("Sandbox")
	debugdir ("../bin/" .. outputdir .. "/%{prj.name}")
	
	
	Link("Core")
	Link("Utils")
	Link("GLRenderer")

	includedirs
	{
		"../Renderer",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"CORE_PLATFORM=CORE_PLATFORM_WIN"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

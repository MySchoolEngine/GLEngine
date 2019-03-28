include "../premakeDefines.lua"

project "Entity"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	SetupProject("Entity")
	
	PrecompiledHeaders("Entity")
	
	Link("Physics")
	Link("Utils")

	includedirs
	{
		"../%{IncludeDir.GLM}",
		"../Core",
		"../vendor/fmt/include",
	}
	
	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"CORE_PLATFORM=CORE_PLATFORM_WIN",
			"BUILD_DLL",
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
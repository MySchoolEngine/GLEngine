include "../premakeDefines.lua"

project "Core"
	kind "SharedLib"
	language "C++"
	staticruntime "off"
	
	SetupProject("Core")
	
	PrecompiledHeaders("Core")

	Link("Utils")

	includedirs
	{
		"../Renderer/",
		"../GLRenderer/",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"CORE_PLATFORM=CORE_PLATFORM_WIN",
			"BUILD_CORE_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

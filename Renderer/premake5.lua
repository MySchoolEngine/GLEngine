include "../premakeDefines.lua"

project "Renderer"
	kind "SharedLib"
	language "C++"
	staticruntime "off"
	
	SetupProject("Renderer")
	
	PrecompiledHeaders("Renderer")
	
	Link("Utils")
	Link("Entity")
	Link("Core")
	Link("GUI")

	includedirs
	{
		"../Core",
		"../Utils",
		"../GLRenderer",
		"../Physics",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.pugixml}",
		"../%{IncludeDir.DevIL}",

		"../vendor/Assimp/include",
		"../vendor/projects/Assimp"
	}

	libdirs
	{
		"../vendor/bin/Debug-windows-x86_64/DevIL-IL/",
	}

	links 
	{ 
		"pugixml",
		"Assimp",
		"DevIL-IL",
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"BUILD_RENDERER_DLL",
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
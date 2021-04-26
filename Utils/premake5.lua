include "../premakeDefines.lua"

project "Utils"
	kind "SharedLib"
	language "C++"
	staticruntime "off"
	cppdialect "C++17"

	SetupProject("Utils")

	includedirs
	{
		"../Core",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.pugixml}",
	}

	links 
	{ 
		"pugixml",
	}

	defines
	{
		"BUILD_UTILS_DLL",
	}

	filter "system:windows"
		systemversion "latest"
		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}
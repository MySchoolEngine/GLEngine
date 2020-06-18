include "../premakeDefines.lua"

project "Utils"
	kind "SharedLib"
	language "C++"
	staticruntime "off"
	
	SetupProject("Utils")

	includedirs
	{
		"../Core",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.pugixml}",
	}
	
	links 
	{ 
		"pugixml",
	}

	filter "system:windows"
		defines
		{
			"BUILD_UTILS_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}
include "../premakeDefines.lua"

project "Renderer"
	kind "SharedLib"
	language "C++"
	staticruntime "off"
	
	SetupProject("Renderer")
	
	PrecompiledHeaders("Renderer")
	
	Link("Utils")
	Link("Entity")

	includedirs
	{
		"../Core",
		"../Utils",
		"../GLRenderer",
		"../Physics",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.pugixml}",
	}

	links 
	{ 
		"pugixml",
	}

	filter "system:windows"
		defines
		{
			"BUILD_RENDERER_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

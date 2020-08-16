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
		"../%{IncludeDir.DevIL}",

		"../vendor/AssimpPrebuild/include",
	}

	libdirs
	{
		"../vendor/AssimpPrebuild/lib/",
		"../vendor/bin/Debug-windows-x86_64/DevIL-IL/",
	}

	links 
	{ 
		"pugixml",
		"../vendor/AssimpPrebuild/lib/assimp.lib",
		"DevIL-IL",
		"../vendor/projects/DevIL/bin/Debug-windows-x86_64/DevIL-IL/DevIL-IL.dll",
	}

	filter "system:windows"
		defines
		{
			"BUILD_RENDERER_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\""),
			("{COPY} \"../vendor/AssimpPrebuild/lib/*\" \"../bin/" .. outputdir .. "/Sandbox/\""),
			("{COPY} \"../vendor/projects/DevIL/bin/Debug-windows-x86_64/DevIL-IL/DevIL-IL.dll\" \"../bin/" .. outputdir .. "/Sandbox/\""),
		}

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
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
	}

	filter "system:windows"
		defines
		{
			"BUILD_CORE_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

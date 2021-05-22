include "../Tools/Premake5/premakeDefines.lua"

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
		systemversion "latest"

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

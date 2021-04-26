include "../premakeDefines.lua"

project "Physics"
	kind "SharedLib"
	language "C++"
	staticruntime "off"
	
	SetupProject("Physics")

	includedirs
	{
		"../Core",
		"../%{IncludeDir.GLM}",
	}

	filter "system:windows"
		defines
		{
			"BUILD_PHYSICS_DLL",
		}

		-- postbuildcommands
		-- {
		-- 	("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		-- }

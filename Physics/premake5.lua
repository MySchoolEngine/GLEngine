include "../Tools/Premake5/premakeDefines.lua"

project "Physics"
	kind "SharedLib"
	language "C++"
	staticruntime "off"
	
	SetupProject("Physics")

	includedirs
	{
		"../Core",
	}
	
	usage "PUBLIC"
		includedirs
		{
			"%{wks.location}/%{IncludeDir.GLM}",
			"%{wks.location}/%{IncludeDir.fmt}",
		}
	usage "INTERFACE"
		includedirs
		{
			"."
		}

	--filter "system:windows"
		-- postbuildcommands
		-- {
		-- 	("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\"")
		-- }

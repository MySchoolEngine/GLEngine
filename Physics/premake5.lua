include "../Tools/Premake5/premakeDefines.lua"

project "Physics"
	kind "SharedLib"
	language "C++"
	staticruntime "off"
	
	SetupProject("Physics")

	includedirs
	{
		"../Core",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
	}

	--filter "system:windows"
		-- postbuildcommands
		-- {
		-- 	("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		-- }

include "../Tools/Premake5/premakeDefines.lua"

project "Physics"
	kind "SharedLib"
	language "C++"
	
	SetupProject("Physics")
	PrecompiledHeaders("Physics")

	Link("Renderer")

	includedirs
	{
		"../Core",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
	}

	--filter "system:windows"
		-- postbuildcommands
		-- {
		-- 	("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\"")
		-- }

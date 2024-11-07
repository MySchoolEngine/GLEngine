include "../Tools/Premake5/premakeDefines.lua"

project "Utils"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	SetupProject("Utils")

	LinkDependency("pugixml")
	LinkDependency("RTTR")

	includedirs
	{
		"../Core",
		"../Physics",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.GLM}",
	}

	filter "system:windows"
		systemversion "latest"
		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\"")
		}
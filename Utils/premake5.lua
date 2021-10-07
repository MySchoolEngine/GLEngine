include "../Tools/Premake5/premakeDefines.lua"

project "Utils"
	kind "SharedLib"
	language "C++"
	staticruntime "off"
	cppdialect "C++17"

	SetupProject("Utils")

	LinkDependency("pugixml")
	LinkDependency("RTTR")

	includedirs
	{
		"../Core",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.GLM}",
	}

	filter "system:windows"
		systemversion "latest"
		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\"")
		}
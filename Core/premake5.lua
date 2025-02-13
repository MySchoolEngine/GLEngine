include "../Tools/Premake5/premakeDefines.lua"

project "Core"
	kind "SharedLib"
	language "C++"
	staticruntime "off"
	
	SetupProject("Core")
	
	PrecompiledHeaders("Core")

	Link("Utils")
	links {"uuid"}
	LinkDependency("crossguid")
	LinkDependency("RTTR")
	LinkDependency("pugixml")

	includedirs
	{
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
		"../Physics",
	}

	filter "system:windows"
		systemversion "latest"

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\"")
		}

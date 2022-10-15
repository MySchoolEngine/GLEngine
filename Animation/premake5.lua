include "../Tools/Premake5/premakeDefines.lua"

project "Animation"
	kind "SharedLib"
	language "C++"
	staticruntime "off"
	
	SetupProject("Animation")
	
	PrecompiledHeaders("Animation")
	
	Link("Utils")
	Link("Core")
	LinkDependency("RTTR")

	includedirs
	{
		"../Physics",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.pugixml}",
		"../%{IncludeDir.ImGui}",
		"../%{IncludeDir.GLFW}", -- for key names
	}

	links 
	{ 
		"pugixml",
		"ImGui",
	}


	filter "system:windows"
		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\""),
		}
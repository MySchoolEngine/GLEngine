include "../../premakeDefines.lua"

project "ShaderPreprocessor"
    kind "ConsoleApp"
	language "C++"
	staticruntime "off"
	
	SetupProject("ShaderPreprocessor")
	debugdir ("%{wks.location}/data/")
	
	
	Link("Core")
	Link("Utils")
	Link("Renderer")
	Link("GLRenderer")
	Link("GUI")

	includedirs
	{
		"%{wks.location}/%{IncludeDir.fmt}",
		"%{wks.location}/%{IncludeDir.pugixml}",
		"%{wks.location}/%{IncludeDir.GLM}",
	}

	links 
	{
		"pugixml",
		"ImGui",
	}

	postbuildcommands
	{
		("{COPY} \"%{wks.location}/vendor/projects/DevIL/bin/Debug-windows-x86_64/DevIL-IL/DevIL-IL.dll\" \"%{cfg.buildtarget.directory}\""),
		("{COPY} \"%{cfg.buildtarget.directory}/../Entity/Entity.dll\" \"%{cfg.buildtarget.directory}\""),
		("{COPY} \"%{wks.location}/bin/" .. outputdir .. "/vendor/ImGui/*.*\" \"%{cfg.buildtarget.directory}\""),
	}

	filter "system:linux"
		links
		{
			"X11",
		}
		linkoptions "-pthread"
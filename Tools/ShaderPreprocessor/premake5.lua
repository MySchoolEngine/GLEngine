include "../../Tools/Premake5/premakeDefines.lua"

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
	Link("Editor")
	LinkDependency("ImGui")
	LinkDependency("pugixml")
	LinkDependency("RTTR")

	includedirs
	{
		"%{wks.location}/%{IncludeDir.fmt}",
		"%{wks.location}/%{IncludeDir.GLM}",
	}
	
	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		postbuildcommands
		{
			("{COPY} \"%{cfg.buildtarget.directory}/../Entity/Entity.dll\" \"%{cfg.buildtarget.directory}\""),
			("{COPY} \"%{wks.location}/bin/" .. outputdir .. "/vendor/ImGui/*.*\" \"%{cfg.buildtarget.directory}\""),
		}

	filter "system:linux"
		links
		{
			"X11",
		}
		linkoptions "-pthread"
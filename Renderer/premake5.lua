include "../Tools/Premake5/premakeDefines.lua"

project "Renderer"
	kind "SharedLib"
	language "C++"
	staticruntime "off"
	
	SetupProject("Renderer")
	
	PrecompiledHeaders("Renderer")
	
	Link("Utils")
	Link("Entity")
	Link("Core")
	Link("GUI")
--	Link("Animation")
	
	LinkDependency("Assimp")
	LinkDependency("ImGui")
	LinkDependency("pugixml")
	LinkDependency("RTTR")

	includedirs
	{
		"%{wks.location}/Physics",
		"%{wks.location}/%{IncludeDir.GLM}",
		"%{wks.location}/%{IncludeDir.GLFW}",
		"%{wks.location}/%{IncludeDir.fmt}",
		"%{wks.location}/%{IncludeDir.DevIL}",

		"%{wks.location}/vendor/projects/Assimp"
	}

	links 
	{
		"DevIL-IL",
	}

	filter "system:windows"
		defines
		{
			"BUILD_RENDERER_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\""),
		}

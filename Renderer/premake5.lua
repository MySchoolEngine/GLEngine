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
	
	LinkDependency("Assimp")
	LinkDependency("ImGui")
	LinkDependency("pugixml")

	includedirs
	{
		"../GLRenderer",
		"../Physics",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.DevIL}",

		"../vendor/projects/Assimp"
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

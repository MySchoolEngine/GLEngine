include "../Tools/Premake5/premakeDefines.lua"

project "Editor"
	kind "SharedLib"
	language "C++"
	staticruntime "off"
	
	SetupProject("Editor")
	
	PrecompiledHeaders("Editor")
	
	Link("Utils")
	Link("Entity")
	Link("Core")
	Link("GUI")
	Link("Renderer")

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
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\""),
		}
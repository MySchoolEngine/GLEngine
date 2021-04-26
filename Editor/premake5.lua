include "../premakeDefines.lua"

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
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.pugixml}",
		"../%{IncludeDir.ImGui}",
	}

	links 
	{ 
		"pugixml",
		"ImGui",
	}

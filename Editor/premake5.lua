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
	Link("Animation")
	
	LinkDependency("ImGui")
	LinkDependency("pugixml")
	LinkDependency("ImGuizmo")

	LinkDependency("RTTR")

	LinkDependency("RTTR")

	includedirs
	{
		"../Physics",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.GLFW}", -- for key names
	}


	filter "system:windows"
		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\""),
		}
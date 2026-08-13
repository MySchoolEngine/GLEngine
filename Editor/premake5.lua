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
--	Link("Animation")
	uses{"Core", "Tracy"}
	
	LinkDependency("ImGui")
	LinkDependency("pugixml")
	LinkDependency("ImGuizmo")
	filter "configurations:Debug"
		LinkDependency("Tracy")
	filter {}

	LinkDependency("RTTR")

	LinkDependency("RTTR")

	includedirs
	{
		"../Physics",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.GLFW}", -- for key names
		"%{wks.location}/%{IncludeDir.slot_map}",
		"%{wks.location}/%{IncludeDir.IconFontCppHeaders}",
	}


	filter "system:windows"
		defines
		{
			"IMGUI_API=__declspec(dllimport)",
		}

		CopyToSandbox()
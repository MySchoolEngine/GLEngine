include "../Tools/Premake5/premakeDefines.lua"

project "Sandbox"
    kind "ConsoleApp"
	language "C++"
	staticruntime "off"
	
	debugdir ("../data/")
	SetupProject("Sandbox")
	
	pchheader "SandboxStdafx.h"
	pchsource "SandboxStdafx.cpp"
	
	Link("Core")
	Link("Utils")
	Link("GLRenderer")
	LinkDependency("RTTR")
	if (_OPTIONS["glfwapi"] ~= "opengl") then
		Link("VulkanRenderer")
	end

	files
	{
		"SandboxStdafx.cpp",
		"SandboxStdafx.h",
	}

	includedirs
	{
		"../Renderer",
		"../GLFWWindowManager",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
	}

	filter "system:windows"
		kind "WindowedApp"
		systemversion "latest"
		Link("DX12Renderer")

	filter "system:linux"
		links
		{
			"X11",
		}
		linkoptions "-pthread"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

include "../premakeDefines.lua"

project "Sandbox"
    kind "ConsoleApp"
	language "C++"
	staticruntime "off"
	
	debugdir ("../data/")
	SetupProject("Sandbox")
	debugdir ("../data/")
	
	
	Link("Core")
	Link("Utils")
	Link("GLRenderer")
	Link("DX12Renderer")
	Link("VulkanRenderer")

	includedirs
	{
		"../Renderer",
		"../GLFWWindowManager",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
	}

	filter "system:windows"
    	kind "WindowedApp"

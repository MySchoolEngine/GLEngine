include "../premakeDefines.lua"

project "Core"
	kind "SharedLib"
	language "C++"
	staticruntime "off"
	
	SetupProject("Core")
	
	PrecompiledHeaders("Core")

	Link("Utils")

	includedirs
	{
		"../Entity/",
		"../Renderer/",
		"../GLRenderer/",
		"../DX12Renderer/",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.GLM}",
	}

	filter "system:windows"
		systemversion "latest"

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

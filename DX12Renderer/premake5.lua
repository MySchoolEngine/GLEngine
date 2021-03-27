include "../premakeDefines.lua"

project "DX12Renderer"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	SetupProject("DX12Renderer")
	
	PrecompiledHeaders("DX12Renderer")

	Link("Core")
	Link("Entity")
	Link("Utils")
	Link("Renderer")

	includedirs
	{
		"../Physics",
		"C:/Program Files (x86)/Windows Kits/10/Include/10.0.10240.0/shared",
		"C:/Program Files (x86)/Windows Kits/10/Include/10.0.10240.0/um",
		"../%{IncludeDir.pugixml}",
		"../%{IncludeDir.GLM}",
	}

	links 
	{ 
		"pugixml",

		-- DX Stuff
		"d3d12",
   		"dxgi",
   		"d3dcompiler",
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"BUILD_DX12RENDERER_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\""),
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
      	defines({ "DEBUG" })

	filter "configurations:Release"
      	defines({ "NDEBUG" })
		runtime "Release"
		optimize "On"

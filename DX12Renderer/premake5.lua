include "../Tools/Premake5/premakeDefines.lua"

project "DX12Renderer"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	SetupProject("DX12Renderer")
	
	PrecompiledHeaders("DX12Renderer")
	
	Link("Entity")
	Link("Utils")
	Link("Renderer")
	Link("Core")

	LinkDependency("pugixml")

	includedirs
	{
		"../Physics",
		"C:/Program Files (x86)/Windows Kits/10/Include/10.0.10240.0/shared",
		"C:/Program Files (x86)/Windows Kits/10/Include/10.0.10240.0/um",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.GLM}",
	}

	links 
	{ 
		-- DX Stuff
		"d3d12",
   		"dxgi",
   		"d3dcompiler",
	}

	filter "system:windows"

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\""),
		}


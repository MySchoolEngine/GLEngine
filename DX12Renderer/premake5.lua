include "../premakeDefines.lua"

project "DX12Renderer"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../obj/" .. outputdir .. "/%{prj.name}")
	
	pchheader "DX12RendererStdafx.h"
	pchsource "DX12RendererStdafx.cpp"

	files
	{
		"DX12Renderer/**.h",
		"DX12Renderer/**.cpp",
		"DX12Renderer/**.inl",
		"DX12RendererStdafx.cpp",
		"DX12RendererStdafx.h",
		"premake5.lua",
	}

	includedirs
	{
		".",
		"../Renderer",
		"../Entity",
		"../Utils",
		"../Physics",
		"../Core",
		"C:/Program Files (x86)/Windows Kits/10/Include/10.0.10240.0/shared",
		"C:/Program Files (x86)/Windows Kits/10/Include/10.0.10240.0/um",
		"../%{IncludeDir.pugixml}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.ImGui}",
		"../%{IncludeDir.GLM}",
	}

	links 
	{ 
		"Physics",
		"pugixml",
		"ImGui",

		"Entity",
		"Utils",
		"Renderer",
		"Core",

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
			"CORE_PLATFORM=CORE_PLATFORM_WIN",
			"BUILD_DLL",
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

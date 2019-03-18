include "../premakeDefines.lua"

project "GLRenderer"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../obj/" .. outputdir .. "/%{prj.name}")
	
	pchheader "GLRendererStdafx.h"
	pchsource "GLRendererStdafx.cpp"

	files
	{
		"GLRenderer/**.h",
		"GLRenderer/**.cpp",
		"GLRenderer/**.inl",
		"GLRendererStdafx.cpp",
		"GLRendererStdafx.h",
		"premake5.lua",
	}

	excludes { "**/Shaders/**" }

	includedirs
	{
		".",
		"../Renderer",
		"../Entity",
		"../Utils",
		"../Physics",
		"../Core",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.Glad}",
		"../%{IncludeDir.GLM}",
		"../vendor/fmt/include",
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"opengl32.lib",

		"Entity",
		"Utils",
		"Renderer",
		"Core",
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
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

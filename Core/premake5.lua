include "../premakeDefines.lua"

project "Core"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../obj/" .. outputdir .. "/%{prj.name}")

	pchheader "CoreStdafx.h"
	pchsource "CoreStdafx.cpp"

	files
	{
		"Core/**.h",
		"Core/**.cpp",
		"Core/**.inl",
		"CoreStdafx.cpp",
		"CoreStdafx.h",
		"premake5.lua",
	}

	includedirs
	{
		".",
		"../Renderer/",
		"../GLRenderer/",
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
		"Renderer",
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

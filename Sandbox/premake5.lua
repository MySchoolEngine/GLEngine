include "../premakeDefines.lua"

project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../obj/" .. outputdir .. "/%{prj.name}")
	debugdir ("../bin/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Sandbox/**.h",
		"Sandbox/**.cpp",
		"Sandbox/**.inl",
		"premake5.lua",
	}

	includedirs
	{
		".",
		"../Core",
		"../Renderer",
		"../Utils",
		"../GLRenderer",
		"../%{IncludeDir.GLM}",
		"../vendor/fmt/include",
	}

	links
	{ 
		"Core",
		"Utils", 
		"GLRenderer",
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"CORE_PLATFORM=CORE_PLATFORM_WIN"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

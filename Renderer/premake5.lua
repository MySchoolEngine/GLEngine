include "../premakeDefines.lua"

project "Renderer"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../obj/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Renderer/**.h",
		"Renderer/**.cpp",
		"Renderer/**.inl",
		"premake5.lua",
	}

	includedirs
	{
		".",
		"../Entity",
		"../vendor/fmt/include",
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"CORE_PLATFORM=CORE_PLATFORM_WIN",
			"BUILD_DLL",
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

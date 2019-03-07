include "../premakeDefines.lua"

project "Entity"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../obj/" .. outputdir .. "/%{prj.name}")

	pchheader "CoreStdafx.h"
	pchsource "src/CoreStdafx.cpp"

	files
	{
		"Entity/**.h",
		"Entity/**.cpp",
		"Entity/**.inl",
		"premake5.lua",
	}

	includedirs
	{
		".",
		"../%{IncludeDir.GLM}",
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

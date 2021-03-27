include "../premakeDefines.lua"

project "Audio"
	kind "SharedLib"
	language "C++"

	SetupProject("Audio")
	
	PrecompiledHeaders("Audio")
	
	Link("Entity")
	Link("Utils")
	Link("Core")

	includedirs
	{
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmod}",
	}

	defines
	{
		"MODULE_CTX=Audio"
	}

	links
	{
		"../%{LibDir.fmod}/fmod_vc.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"BUILD_AUDIO_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\""),
			("{COPY} \"../%{LibDir.fmod}/fmod.dll\" \"../bin/" .. outputdir .. "/Sandbox/\""),
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
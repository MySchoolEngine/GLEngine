include "../premakeDefines.lua"

project "Physics"
	kind "SharedLib"
	language "C++"
	staticruntime "off"
	
	SetupProject("Physics")

	includedirs
	{
		"../Core",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"BUILD_PHYSICS_DLL",
		}

		-- postbuildcommands
		-- {
		-- 	("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		-- }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

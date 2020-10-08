include "../../premakeDefines.lua"

project "pugixml"
	kind "StaticLib"
	language "C++"


	targetdir ("../../bin/" .. outputdir .. "/vendor/%{prj.name}")
	objdir ("../../obj/" .. outputdir .. "/vendor/%{prj.name}")

	files { 
		"../pugixml/src/pugixml.hpp", 
		"../pugixml/src/pugiconfig.hpp",
		"../pugixml/src/pugixml.cpp" }

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		defines { "_DEBUG" }

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		defines { "NDEBUG" }

project "crossguid"
	kind "StaticLib"
	language "C++"


	targetdir ("%{wks.location}/bin/" .. outputdir .. "/vendor/%{prj.name}")
	objdir ("%{wks.location}/obj/" .. outputdir .. "/vendor/%{prj.name}")

	files { 
		"%{wks.location}/vendor/%{prj.name}/src/guid.cpp", 
		"%{wks.location}/vendor/%{prj.name}/include/crossguid/guid.hpp"
	}

	includedirs
	{
		"%{wks.location}/vendor/%{prj.name}/include/"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		defines {"GUID_WINDOWS"}

	filter "system:linux"
		pic "On"
		defines {"GUID_LIBUUID"}
		libdirs { os.findlib("uuid") }
		links {"uuid"}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		defines { "_DEBUG" }

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		defines { "NDEBUG" }

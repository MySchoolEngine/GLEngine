include "../../Tools/Premake5/premakeDefines.lua"

project "UtilsTest"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	SetupProject("UtilsTest")
	
	Link("Utils")
	links { "gtest_main" }

	LinkDependency("RTTR")

	includedirs
	{
		"%{wks.location}/vendor/gtest/googletest/include",
		"%{wks.location}/Utils",
		"%{wks.location}/%{IncludeDir.GLM}",
		"%{wks.location}/%{IncludeDir.fmt}",
		"%{wks.location}/%{IncludeDir.RTTR}",
	}
	files
	{
		"%{wks.location}/vendor/gtest/googletest/src/gtest_main.cc",
	}

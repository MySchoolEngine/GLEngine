include "../../Tools/Premake5/premakeDefines.lua"

project "CoreTest"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	SetupProject("CoreTest")
	
	Link("Utils")
	Link("Core")
	links { "gtest_main" }

	LinkDependency("RTTR")

	includedirs
	{
		"%{wks.location}/vendor/gtest/googletest/include",
		"%{wks.location}/Core",
		"%{wks.location}/Utils",
		"%{wks.location}/%{IncludeDir.GLM}",
		"%{wks.location}/%{IncludeDir.fmt}",
		"%{wks.location}/%{IncludeDir.RTTR}",
	}
	files
	{
		"%{wks.location}/vendor/gtest/googletest/src/gtest_main.cc",
	}

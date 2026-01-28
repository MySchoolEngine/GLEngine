include "../../Tools/Premake5/premakeDefines.lua"

project "CoreTest"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	SetupProject("CoreTest")
	
	PrecompiledHeaders("CoreTest")
	
	Link("Utils")
	Link("Core")
	links { "gtest_main" }

	LinkDependency("RTTR")
	LinkDependency("pugixml")

	uses {"DULib"}

	includedirs
	{
		"%{wks.location}/vendor/gtest/googletest/include",
		"%{wks.location}/Core",
		"%{wks.location}/Utils",
		"%{wks.location}/Physics",
		"%{wks.location}/%{IncludeDir.GLM}",
		"%{wks.location}/%{IncludeDir.fmt}",
		"%{wks.location}/%{IncludeDir.RTTR}",
		"%{wks.location}/Tests/CommonTestUtils",
	}
	files
	{
		"%{prj.location}/TestMain.cpp",
	}

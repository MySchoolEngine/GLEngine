include "../../Tools/Premake5/premakeDefines.lua"

project "UtilsTest"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	SetupProject("UtilsTest")
	
	PrecompiledHeaders("UtilsTest")
	
	Link("Core")
	Link("Utils")
	links { "gtest_main" }

	LinkDependency("RTTR")
	LinkDependency("pugixml")

	includedirs
	{
		"%{wks.location}/vendor/gtest/googletest/include",
		"%{wks.location}/Utils",
		"%{wks.location}/%{IncludeDir.GLM}",
		"%{wks.location}/%{IncludeDir.fmt}",
		"%{wks.location}/%{IncludeDir.RTTR}",
		"%{wks.location}/Tests/CommonTestUtils",
	}
	files
	{
		"%{prj.location}/TestMain.cpp",
	}

	filter "action:vs*"
		buildoptions { "/bigobj" }

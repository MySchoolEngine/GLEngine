include "../../Tools/Premake5/premakeDefines.lua"

project "AITest"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	SetupProject("AITest")

	PrecompiledHeaders("AITest")

	Link("AI")
	Link("Utils")
	links { "gtest_main" }

	LinkDependency("RTTR")

	includedirs
	{
		"%{wks.location}/vendor/gtest/googletest/include",
		"%{wks.location}/vendor/gtest/googlemock/include",
		"%{wks.location}/Entity",
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

	CopyLib("Core")

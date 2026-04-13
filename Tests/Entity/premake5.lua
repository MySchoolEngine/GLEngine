include "../../Tools/Premake5/premakeDefines.lua"

project "EntityTest"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	SetupProject("EntityTest")

	PrecompiledHeaders("EntityTest")

	Link("Entity")
	Link("Core")
	Link("Utils")
	Link("GUI")
	links { "gtest_main" }

	uses
	{
		"Entity",
		"Core",
		"Utils",
		"GUI",
	}

	LinkDependency("RTTR")
	LinkDependency("pugixml")

	CopyDependencyLib("ImGui")

	includedirs
	{
		"%{wks.location}/vendor/gtest/googletest/include",
		"%{wks.location}/Core",
		"%{wks.location}/Utils",
		"%{wks.location}/Entity",
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

include "../../Tools/Premake5/premakeDefines.lua"

project "PhysicsTest"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	SetupProject("PhysicsTest")

	PrecompiledHeaders("PhysicsTest")

	links { "gtest_main" }

	uses { "Physics" }
	
	Link("Utils")
	Link("Core")

	LinkDependency("RTTR")
	filter "configurations:Debug"
		LinkDependency("Tracy")
	filter {}

	uses { "Tracy" }

	includedirs
	{
		"%{wks.location}/vendor/gtest/googletest/include",
		"%{wks.location}/Tests/CommonTestUtils",
	}
	files
	{
		"%{prj.location}/TestMain.cpp",
	}

include "../../Tools/Premake5/premakeDefines.lua"

project "RendererTest"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	SetupProject("RendererTest")

	PrecompiledHeaders("RendererTest")

	Link("Renderer")
	Link("Utils")
	Link("Entity")
	Link("Core")
	Link("GUI")
	links { "gtest_main" }

	LinkDependency("RTTR")
	LinkDependency("pugixml")

	includedirs
	{
		"%{wks.location}/vendor/gtest/googletest/include",
		"%{wks.location}/vendor/gtest/googlemock/include",
		"%{wks.location}/Core",
		"%{wks.location}/Utils",
		"%{wks.location}/Physics",
		"%{wks.location}/%{IncludeDir.GLM}",
		"%{wks.location}/%{IncludeDir.fmt}",
		"%{wks.location}/%{IncludeDir.RTTR}",
		"%{wks.location}/%{IncludeDir.slot_map}",
		"%{wks.location}/Tests/CommonTestUtils",
	}
	files
	{
		"%{prj.location}/TestMain.cpp",
	}

	CopyLib("Core")
	CopyLib("Entity")
	CopyLib("GUI")
	CopyDependencyLib("Assimp")
	CopyDependencyLib("zlib")
	CopyDependencyLib("ImGui")
	CopyDependencyLib("pugixml")
	CopyDependencyLib("DevIL-IL")

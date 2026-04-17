include "../../Tools/Premake5/premakeDefines.lua"

project "RendererTest"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	SetupProject("RendererTest")

	PrecompiledHeaders("RendererTest")

	-- Link against RendererStatic instead of Renderer DLL
	
	-- for GCC we need to compile either everything or nothing statically
	-- Static linkage allows us to test non-DLL exported types
	filter "action:gmake*"
		Link("Renderer", false)  -- true = static library
	filter "action:vs*"
		Link("Renderer", true)  -- true = static library
	filter {}
	Link("Utils")
	Link("Entity")
	Link("Core")
	Link("GUI")
	links { "gtest_main" }
	uses
	{
		"Renderer",
		"Utils",
		"Entity",
		"Core",
		"GUI",
	}

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

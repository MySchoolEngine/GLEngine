include "../../Tools/Premake5/premakeDefines.lua"

project "RendererTest"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	SetupProject("RendererTest")
	
	Link("Renderer")
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
		"**.cpp",
		"%{wks.location}/vendor/gtest/googletest/src/gtest_main.cc",
	}

	CopyLib("Core")
	CopyLib("Entity")
	CopyLib("Utils")
	CopyLib("GUI")
	CopyDependencyLib("Assimp")
	CopyDependencyLib("ImGui")
	CopyDependencyLib("pugixml")
	CopyDependencyLib("DevIL-IL")

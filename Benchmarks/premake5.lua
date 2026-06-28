include "../Tools/Premake5/premakeDefines.lua"

project "Benchmarks"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"
	
	SetupProject("Benchmarks")

	files
	{
		"%{prj.location}/**.h",
		"%{prj.location}/**.cpp",
		"premake5.lua",
	}
	
	uses
	{
		"DULib",
		"Physics",
		"Tracy",
	}

	-- Link against RendererStatic to access non-exported types (e.g. BVH)
	-- (mirrors RendererTest which uses the same pattern for the same reason)
	filter "action:gmake*"
		Link("Renderer", false)
	filter "action:vs*"
		Link("Renderer", true)
	filter {}

	Link("Utils")
	Link("Core")
	Link("Entity")
	Link("GUI")
	filter "configurations:Debug"
		LinkDependency("Tracy")
	filter {}

	links { "benchmark_main" }
	libdirs { "%{wks.location}/bin/" .. outputdir .. "/vendor/benchmark_main" }

	LinkDependency("RTTR")
	LinkDependency("pugixml")

	defines { "RENDERER_STATIC_BUILD", "BENCHMARK_STATIC_DEFINE" }

	includedirs
	{
		"%{wks.location}/vendor/benchmark/include",
		"%{wks.location}/%{IncludeDir.GLM}",
		"%{wks.location}/%{IncludeDir.fmt}",
		"%{wks.location}/%{IncludeDir.Tracy}",
		"%{wks.location}/%{IncludeDir.slot_map}",
		"%{wks.location}/%{IncludeDir.RTTR}",
	}

	filter "system:windows"
		cppdialect "C++23"
		systemversion "latest"
		links { "Shlwapi" }
		CopyDependencyLib("Assimp")
		CopyDependencyLib("zlib")
		CopyDependencyLib("ImGui")
		CopyDependencyLib("DevIL-IL")

	filter "system:linux"
		links { "pthread" }

	filter "action:vs*"
		buildoptions { "/MP" }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		defines { "_DEBUG", "DEBUG" }

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		defines { "NDEBUG" }

	filter "configurations:ASAN"
		runtime "Release"
		optimize "On"
		defines { "NDEBUG" }

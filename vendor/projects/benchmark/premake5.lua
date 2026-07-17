project "benchmark_main"
	kind "StaticLib"
	language "C++"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/vendor/%{prj.name}")
	objdir ("%{wks.location}/obj/" .. outputdir .. "/vendor/%{prj.name}")

	includedirs
	{
		"%{wks.location}/vendor/benchmark/include",
		"%{wks.location}/vendor/benchmark/src",
	}

	files
	{
		"%{wks.location}/vendor/benchmark/src/*.cc",
		"%{wks.location}/vendor/benchmark/src/*.h",
	}

	defines { "BENCHMARK_STATIC_DEFINE" }

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		links { "Shlwapi" }

	filter "system:linux"
		pic "On"
		links { "pthread" }

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

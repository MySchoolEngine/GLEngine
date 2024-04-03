project "gtest_main"
	kind "StaticLib"
	language "C++"


	targetdir ("%{wks.location}/bin/" .. outputdir .. "/vendor/%{prj.name}")
	objdir ("%{wks.location}/obj/" .. outputdir .. "/vendor/%{prj.name}")

	includedirs { "%{wks.location}/vendor/gtest/googletest/include", "%{wks.location}/vendor/gtest/googletest" }
	files { "%{wks.location}/vendor/gtest/googletest/src/gtest-all.cc","%{wks.location}/vendor/gtest/googletest/src/gtest_main.cc", "%{wks.location}/vendor/gtest/googletest/src/*.h" }

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "system:windows"
		defines {"GTEST_OS_WINDOWS"}
	filter "system:linux"
		pic "On"
		defines {"GTEST_OS_LINUX"}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		defines { "_DEBUG", "DEBUG" }

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		defines { "NDEBUG" }
include "../premakeDefines.lua"

project "GLRenderer"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../obj/" .. outputdir .. "/%{prj.name}")
	
	pchheader "GLRendererStdafx.h"
	pchsource "GLRendererStdafx.cpp"
	
	Links("Entity")
	Links("Utils")
	Links("Renderer")
	Links("Core")
	Links("Physics")

	files
	{
		"GLRenderer/**.h",
		"GLRenderer/**.cpp",
		"GLRenderer/**.inl",
		"../%{IncludeDir.ImGui}/examples/imgui_impl_glfw.h",
		"../%{IncludeDir.ImGui}/examples/imgui_impl_opengl3.h",
		"../%{IncludeDir.ImGui}/examples/imgui_impl_glfw.cpp",
		"../%{IncludeDir.ImGui}/examples/imgui_impl_opengl3.cpp",
		"GLRendererStdafx.cpp",
		"GLRendererStdafx.h",
		"premake5.lua",
	}

	includedirs
	{
		".",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.Glad}",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.pugixml}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.ImGui}",
		"../%{IncludeDir.DevIL}",
		
		"../vendor/AssimpPrebuild/include",
	}

	libdirs
	{
		"../vendor/AssimpPrebuild/lib/",
		"../vendor/bin/Debug-windows-x86_64/DevIL-IL/",
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"opengl32.lib",
		"pugixml",
		"ImGui",
		"DevIL-IL",
		"../vendor/AssimpPrebuild/lib/assimp.lib",
		"../vendor/projects/DevIL/bin/Debug-windows-x86_64/DevIL-IL/DevIL-IL.dll",
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"CORE_PLATFORM=CORE_PLATFORM_WIN",
			"BUILD_GLRENDERER_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\""),
			("{COPY} \"../vendor/projects/DevIL/bin/Debug-windows-x86_64/DevIL-IL/DevIL-IL.dll\" \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

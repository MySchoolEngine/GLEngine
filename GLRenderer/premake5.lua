include "../premakeDefines.lua"

project "GLRenderer"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../obj/" .. outputdir .. "/%{prj.name}")
	
	pchheader "GLRendererStdafx.h"
	pchsource "GLRendererStdafx.cpp"
	
	Link("Entity")
	Link("Utils")
	Link("Renderer")
	Link("GLFWWindowManager")
	Link("Core")

	files
	{
		"GLRenderer/**.h",
		"GLRenderer/**.cpp",
		"GLRenderer/**.inl",
		"GLRendererStdafx.cpp",
		"GLRendererStdafx.h",
		"premake5.lua",
	}

	includedirs
	{
		".",
		"../Physics",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.Glad}",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.GLI}",
		"../%{IncludeDir.pugixml}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.ImGui}",
	}

	libdirs
	{
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
		"../vendor/projects/DevIL/bin/Debug-windows-x86_64/DevIL-IL/DevIL-IL.dll",
	}

	defines
	{
		"IMGUI_IMPL_OPENGL_LOADER_GLAD"
	}

	filter "system:windows"
		defines
		{
			"BUILD_GLRENDERER_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\""),
			("{COPY} \"../vendor/projects/DevIL/bin/Debug-windows-x86_64/DevIL-IL/DevIL-IL.dll\" \"../bin/" .. outputdir .. "/Sandbox/\""),
		}

include "../premakeDefines.lua"

project "VulkanRenderer"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	SetupProject("VulkanRenderer")
	PrecompiledHeaders("VulkanRenderer")
	
	Link("Renderer")
	Link("Utils")
	Link("GLFWWindowManager")
	Link("Core")
	Link("Entity")

	includedirs
	{
		".",
		"../Physics",
		"%{VULKAN_SDK}/Include",
		"../%{IncludeDir.pugixml}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.ImGui}",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.GLFW}",
	}

	libdirs { "%{VULKAN_SDK}/Lib" }

	links 
	{ 
		"pugixml",
		"ImGui",
		"vulkan-1.lib",
	}

	filter "system:windows"
		defines
		{
			"BUILD_VULKAN_RENDERER_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\""),
		}

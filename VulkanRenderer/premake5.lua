include "../Tools/Premake5/premakeDefines.lua"

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
	LinkDependency("pugixml")
	LinkDependency("ImGui")

	includedirs
	{
		".",
		"../Physics",
		GetVulkanBasePath().."/Include",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.GLFW}",
	}

	libdirs { GetVulkanBasePath().."/Lib" }

	links 
	{
		"$(VULKAN_SDK)/lib/vulkan-1.lib",
		"vulkan-1.lib",
	}

	filter "system:windows"

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\""),
		}

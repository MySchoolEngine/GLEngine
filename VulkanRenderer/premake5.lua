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
	Link("GUI")
	LinkDependency("pugixml")
	LinkDependency("ImGui")
	LinkDependency("RTTR")

	includedirs
	{
		".",
		"../Physics",
		GetVulkanBasePath().."/Include",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.GLFW}",
		"%{wks.location}/%{IncludeDir.slot_map}",
	}

	libdirs { GetVulkanBasePath().."/Lib" }

	links 
	{ 
		"vulkan-1.lib",
	}

	filter "system:windows"

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\""),
		}

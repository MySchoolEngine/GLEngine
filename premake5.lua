include "Tools/Premake5/workspaceFiles.lua"

newoption {
	trigger = "glfwapi",
	description = "Which graphics API will be used with GLFW",
	default = "opengl",
	allowed = {
      { "opengl", "OpenGL" },
      { "vulkan", "Vulkan (Windows only)" },
   },
}

VULKAN_SDK = os.getenv("VULKAN_SDK")

function GetVulkanBasePath()
	if VULKAN_SDK ~= nil and os.isdir(VULKAN_SDK) then
		return "%{VULKAN_SDK}" end
	return "%{wks.location}/vendor/vulkan"
end

function GetVulkanBin()
	return GetVulkanBasePath().."/Bin"
end

workspace "Engine"
	architecture "x64"
	startproject "Sandbox"
	cppdialect "C++20"
	systemversion "latest"

	configurations{
		"Debug",
		"Release"
	}
  
	defines{
		"FMT_HEADER_ONLY=1",
		"CORE_PLATFORM_WIN=1",
		"CORE_PLATFORM_LINUX=2",
		"GRAPHICS_API_OPENGL=1",
		"GRAPHICS_API_VULKAN=2",
		"GRAPHICS_API_D3D12=3",
		"GLM_ENABLE_EXPERIMENTAL",
		"VULKAN_BIN=\"".. GetVulkanBin() .."\"",
		"VULKAN_GLSLC=VULKAN_BIN \"/glslc.exe\"",
		"IMGUI_DEFINE_MATH_OPERATORS",
		"RTTR_DLL",
	}
	
	workspace_files{
		"vendor/GLM/util/glm.natvis",
		"vendor/projects/crossguid/crossguid.natvis",
		"vendor/ImGui/misc/natvis/imgui.natvis",
		"vendor/pugixml/contrib/natvis/pugixml.natvis",
		"vendor/projects/RTTR/rttr.natvis",
		"vendor/projects/RTTR/rttr.natvis",
		"Tools/Premake5/premakeDefines.lua",
		"Tools/Premake5/workspaceFiles.lua",
		"Animation/animation.natvis",
		"premake5.lua",
		"CHANGELOG.md",
		"README.md",
		"CONTRIBUTING.md",
	}

	filter "options:glfwapi=vulkan"
  		defines{
			"GLENGINE_GLFW_RENDERER=GRAPHICS_API_VULKAN",
  		}
	filter "options:glfwapi=opengl"
  		defines{
			"GLENGINE_GLFW_RENDERER=GRAPHICS_API_OPENGL",
		}

	filter "system:windows"
		disablewarnings {"4251"}
		defines {
			"CORE_PLATFORM=CORE_PLATFORM_WIN",
			"WIN32", 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS",
		}

	filter "action:vs*"
		buildoptions
		{
			"/MP"
		}

	filter "system:linux"
		defines {
			"CORE_PLATFORM=CORE_PLATFORM_LINUX"
		}
		links { "stdc++fs" }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		defines { 
			"DEBUG",
			"GL_ENGINE_DEBUG",
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		defines({ "NDEBUG" })

include "Tools/Premake5/premakeDefines.lua"

IncludeDir = {}
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["GLM"] = "vendor/GLM"
IncludeDir["GLI"] = "vendor/gli"
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["pugixml"] = "vendor/pugixml/src"
IncludeDir["fmt"] = "vendor/fmt/include"
IncludeDir["ImGui"] = "vendor/ImGui"
IncludeDir["ImGuiFileDialog"] = "vendor/ImGuiFileDialog"
IncludeDir["ImGuizmo"] = "vendor/ImGuizmo"
IncludeDir["DevIL"] = "vendor/DevIL/DevIL/include"
IncludeDir["dirent"] = "vendor/dirent/include"
IncludeDir["Assimp"] = "vendor/Assimp/include"
IncludeDir["crossguid"] = "vendor/crossguid/include"
IncludeDir["RTTR"] = {"vendor/RTTR/src", "vendor/projects/RTTR"}
IncludeDir["slot_map"] = "vendor/slot_map"

-- could be header only or static lib
NonDllLib = {}
NonDllLib["pugixml"] = true
NonDllLib["crossguid"] = true
NonDllLib["GLFW"] = true

group "Tests"
	include "Tests/Core"
	include "Tests/Renderer"
	include "Tests/Utils"
group "Dependencies"
  include "vendor/GLFW"
  include "vendor/Glad"
  include "vendor/projects/crossguid"
  include "vendor/projects/DevIL"
  include "vendor/projects/gtest"
  include "vendor/projects/ImGui"
  include "vendor/projects/ImGuiFileDialog"
  include "vendor/projects/ImGuizmo"
  include "vendor/projects/libjpeg"
  include "vendor/projects/libpng"
  include "vendor/projects/pugixml"
  include "vendor/projects/RTTR"
if _TARGET_OS ~= "linux" then
  include "vendor/projects/dirent"
end
group "Dependencies/Assimp"
  include "vendor/projects/zlib"
  include "vendor/projects/Assimp"
group ""

group "Renderes"
	include "GLFWWindowManager"
if _TARGET_OS ~= "linux" then
	include "DX12Renderer"
end
group ""

group "Tools"
	include "Tools/ShaderPreprocessor"
group ""

--include "Animation"
include "Core"
include "Sandbox"
include "Editor"
include "Renderer"
include "GLRenderer"
include "GUI"
if (_OPTIONS["glfwapi"] ~= "opengl") then
	include "VulkanRenderer"
end
include "Entity"
include "Utils"
include "Physics"

require('vstudio')
premake.api.register {
  name = "workspace_files",
  scope = "workspace",
  kind = "list:string",
}

premake.override(premake.vstudio.sln2005, "projects", function(base, wks)
  if wks.workspace_files and #wks.workspace_files > 0 then
    premake.push('Project("{2150E333-8FDC-42A3-9474-1A3956D46DE8}") = "Solution Items", "Solution Items", "{' .. os.uuid("Solution Items:"..wks.name) .. '}"')
    premake.push("ProjectSection(SolutionItems) = preProject")
    for _, path in ipairs(wks.workspace_files) do
      premake.w(path.." = "..path)
    end
    premake.pop("EndProjectSection")
    premake.pop("EndProject")
  end
  base(wks)
end)


workspace "Engine"
	architecture "x64"
	startproject "Sandbox"
	cppdialect "C++17"
	
	configurations{
		"Debug",
		"Release"
	}
  
  defines{
    "FMT_HEADER_ONLY=1",
    "GLENGINE_GLFW_RENDERER=VULKAN",
    "VULKAN_BIN=\"C:/VulkanSDK/Bin\"",
    "VULKAN_GLSLC=VULKAN_BIN \"/glslc.exe\""
  }

	workspace_files{
		"vendor/GLM/util/glm.natvis",
		"premake5.lua",
		"premakeDefines.lua",
	}

	disablewarnings {"4251"}

	filter "action:vs*"
		defines {
			"CORE_PLATFORM=CORE_PLATFORM_WIN",
			"WIN32", 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS",
		}

	filter "action:gmake"
		defines {
			"_GLFW_X11",
		}

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

include "premakeDefines.lua"

IncludeDir = {}
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["GLM"] = "vendor/GLM"
IncludeDir["GLI"] = "vendor/gli"
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["pugixml"] = "vendor/pugixml/src"
IncludeDir["fmt"] = "vendor/fmt/include"
IncludeDir["ImGui"] = "vendor/ImGui"
IncludeDir["ImGuiFileDialog"] = "vendor/ImGuiFileDialog"
IncludeDir["DevIL"] = "vendor/DevIL/DevIL/include"
IncludeDir["dirent"] = "vendor/dirent/include"

VulkanSDKBase = "C:/VulkanSDK/"

group "Dependencies"
  include "vendor/GLFW"
  include "vendor/Glad"
  include "vendor/pugixmlPremake"
  include "vendor/projects/ImGui"
  include "vendor/projects/ImGuiFileDialog"
  include "vendor/projects/DevIL"
  filter "action:vs*"
  	include "vendor/projects/dirent"
group ""
group "Renderes"
	include "GLFWWindowManager"
	include "DX12Renderer"
group ""
group "Tools"
	include "Tools/ShaderPreprocessor"
group ""

include "Core"
include "Sandbox"
include "Renderer"
include "GLRenderer"
include "GUI"
include "VulkanRenderer"
include "Entity"
include "Utils"
include "Physics"

filter "action:vs*"
  include "DX12Renderer"
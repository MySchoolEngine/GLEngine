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
	
	configurations{
		"Debug",
		"Release"
	}
  
	defines{
		"FMT_HEADER_ONLY=1",
	}

	workspace_files{
		"vendor/GLM/util/glm.natvis"
	}

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
    defines "GL_ENGINE_DEBUG"

include "premakeDefines.lua"

IncludeDir = {}
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["GLM"] = "vendor/GLM"
IncludeDir["GLI"] = "vendor/gli"
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["pugixml"] = "vendor/pugixml/src"
IncludeDir["fmt"] = "vendor/fmt/include"
IncludeDir["ImGui"] = "vendor/ImGui"
IncludeDir["DevIL"] = "vendor/DevIL/DevIL/include"
group "Dependencies"
  include "vendor/GLFW"
  include "vendor/Glad"
  include "vendor/pugixmlPremake"
  include "vendor/ImGui"
  include "vendor/projects/DevIL"
group ""

include "Core"
include "Sandbox"
include "Renderer"
include "GLRenderer"
include "Entity"
include "Utils"
include "Physics"

filter "action:vs*"
  include "DX12Renderer"
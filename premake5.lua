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

  filter "configurations:Debug"
    defines "GL_ENGINE_DEBUG"

include "premakeDefines.lua"

IncludeDir = {}
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["GLM"] = "vendor/GLM"
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["pugixml"] = "vendor/pugixml/src"
IncludeDir["fmt"] = "vendor/fmt/include"
group "Dependencies"
  include "vendor/GLFW"
  include "vendor/Glad"
group ""

include "Core"
include "Sandbox"
include "Renderer"
include "GLRenderer"
include "Entity"
include "Utils"
include "Physics"
include "vendor/pugixmlPremake"
--include "GLEngine"
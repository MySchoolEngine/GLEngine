include "../Tools/Premake5/premakeDefines.lua"

-- Helper function to create Renderer project (DLL or Static)
-- isStatic == true => Generate production DLL
function CreateRendererProject(projectName, isStatic)
	project(projectName)
		if isStatic then
			kind "StaticLib"
			defines { "RENDERER_STATIC_BUILD" }
		else
			kind "SharedLib"
		end

		language "C++"

		SetupProject("Renderer")  -- Uses "Renderer" as source path

		PrecompiledHeaders("Renderer")

		Link("Utils")
		Link("Entity")
		Link("Core")
		Link("GUI")
--		Link("Animation")

		LinkDependency("Assimp")
		LinkDependency("ImGui")
		LinkDependency("pugixml")
		LinkDependency("RTTR")

		includedirs
		{
			"%{wks.location}/Physics",
			"%{wks.location}/%{IncludeDir.GLM}",
			"%{wks.location}/%{IncludeDir.GLFW}",
			"%{wks.location}/%{IncludeDir.fmt}",
			"%{wks.location}/%{IncludeDir.DevIL}",
			"%{wks.location}/%{IncludeDir.slot_map}",

			"%{wks.location}/vendor/projects/Assimp"
		}

		links
		{
			"DevIL-IL",
		}

		filter "system:windows"
			if not isStatic then
				defines { "BUILD_RENDERER_DLL" }
				postbuildcommands
				{
					("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\"")
				}
			end
		filter {}
end


if not _OPTIONS["skiptests"] then
	group "Tests/StaticLibs"
		CreateRendererProject("RendererStatic", true)
	group""
end
CreateRendererProject("Renderer", false)
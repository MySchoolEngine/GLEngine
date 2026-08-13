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

		if isStatic then
			-- RendererStatic only needs Utils/Entity/Core/GUI/RTTR/pugixml's headers to
			-- compile its own sources - it must not link their DLL import libraries into
			-- its own archive. LIB.exe (unlike the linker) doesn't fold the duplicate
			-- __NULL_IMPORT_DESCRIPTOR placeholder every DLL import library carries, so
			-- embedding them produces spurious LNK4006 warnings for anyone who links
			-- RendererStatic. The real consumers (RendererTest, Benchmarks) already
			-- Link()/LinkDependency() these particular dependencies themselves.
			--
			-- Assimp/ImGui/DevIL-IL are different: those consumers do NOT link them
			-- independently, and rely on RendererStatic embedding those import libraries
			-- so their symbols (e.g. ImGui::Text, aiGetMaterialColor, ilLoadImage) resolve
			-- transitively - so those three must stay linked here (still triggering
			-- LNK4006 for those three specifically, which is unavoidable while consumers
			-- depend on the transitive link).
			includedirs
			{
				"%{wks.location}/Utils",
				"%{wks.location}/Entity",
				"%{wks.location}/Core",
				"%{wks.location}/GUI",
				"%{wks.location}/%{IncludeDir.pugixml}",
				"%{wks.location}/vendor/RTTR/src",
				"%{wks.location}/vendor/projects/RTTR",
			}
		else
			Link("Utils")
			Link("Entity")
			Link("Core")
			Link("GUI")
	--		Link("Animation")

			LinkDependency("pugixml")
			LinkDependency("RTTR")
		end

		LinkDependency("Assimp")
		LinkDependency("ImGui")

		links
		{
			"DevIL-IL",
		}

		filter "configurations:Debug"
			LinkDependency("Tracy")
		filter {}

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

		filter "system:windows"
			if not isStatic then
				defines { "BUILD_RENDERER_DLL" }
				CopyToSandbox()
			end
		filter {}

		usage "PUBLIC"
			uses{"DULib", "Tracy"}
end


if not _OPTIONS["skiptests"] or _OPTIONS["benchmarks"] then
	-- currently we do not have static libraries support on GCC
	-- they compile, but are not used, for more refere to RendererTest/premake5.lua
	-- RendererStatic is also needed by Benchmarks to access non-exported types (e.g. BVH)
	filter "action:vs*"
		group "Tests/StaticLibs"
			CreateRendererProject("RendererStatic", true)
		group""
	filter{}
end
CreateRendererProject("Renderer", false)
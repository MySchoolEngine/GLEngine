include "../premakeDefines.lua"

project "Entity"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	SetupProject("Entity")
	
	PrecompiledHeaders("Entity")
	
	Link("Utils")
	Link("Core")
	Link("GUI")

	includedirs
	{
		"../%{IncludeDir.GLM}",
		"../Renderer",
		"../GLRenderer",
		"../Physics",
		"../%{IncludeDir.pugixml}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.ImGui}",
	}

	links{
		"pugixml",
		"ImGui",
	}

	defines
	{
		"MODULE_CTX=Entity"
	}

	filter "system:windows"
		defines
		{
			"BUILD_ENTITY_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

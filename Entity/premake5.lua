include "../Tools/Premake5/premakeDefines.lua"

project "Entity"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	SetupProject("Entity")

	PrecompiledHeaders("Entity")

	Link("Utils")
	Link("Core")
	Link("GUI")

	LinkDependency("ImGui")
	LinkDependency("pugixml")
	LinkDependency("RTTR")

	includedirs
	{
		"../Renderer",
		"../GLRenderer",
		"../Physics",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
	}

	defines
	{
		"MODULE_CTX=Entity"
	}

	filter "system:windows"
		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\"")
		}
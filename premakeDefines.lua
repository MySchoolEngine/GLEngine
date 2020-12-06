outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

function PrecompiledHeaders(projectName)
	pchheader (tostring(projectName).."Stdafx.h")
	pchsource (tostring(projectName).."Stdafx.cpp")

	files 
	{
		tostring(projectName).."Stdafx.cpp",
		tostring(projectName).."Stdafx.h"
	}

end

function ProjectFiles(projectName)
	files
	{
		tostring(projectName).."/**.h",
		tostring(projectName).."/**.cpp",
		tostring(projectName).."/**.inl",
		"premake5.lua",
	}

	includedirs
	{
		".",
	}
end

function Link(projectName)
	includedirs("../" .. projectName)

	links
	{
		projectName,
	}
end

function SetupProject(projectName)
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../obj/" .. outputdir .. "/%{prj.name}")
	ProjectFiles(projectName)
	defines
	{
		"BUILD_"..string.upper(tostring(projectName)).."_DLL",
	}
end
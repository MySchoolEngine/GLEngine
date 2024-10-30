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
		"%{prj.location}/"..tostring(projectName).."/**.h",
		"%{prj.location}/"..tostring(projectName).."/**.cpp",
		"%{prj.location}/"..tostring(projectName).."/**.inl",
		"premake5.lua",
	}

	includedirs
	{
		".",
	}
end

function CopyLib(projectName)
if _TARGET_OS == "windows" then
	postbuildcommands
	{
		("{COPY} \"%{wks.location}/bin/" .. outputdir .. "/" .. projectName .. "/".. projectName ..".dll\" \"%{cfg.buildtarget.directory}\"")
	}
end
end

function CopyDependencyLib(depName)
	if _TARGET_OS == "windows" then
	  if NonDllLib[depName] == nil then
		postbuildcommands
		{
			("{COPY} \"%{wks.location}/bin/" .. outputdir .. "/vendor/" .. depName .. "/".. depName ..".dll\" \"%{cfg.buildtarget.directory}\"")
		}
	  else
		postbuildcommands
		{
			("{COPY} \"%{wks.location}/bin/" .. outputdir .. "/vendor/" .. depName .. "/".. depName ..".lib\" \"%{cfg.buildtarget.directory}\"")
		}
	  end
	end
end

function Link(projectName)
	includedirs("%{wks.location}/" .. projectName)
	-- error("%{prj.location}")
	libdirs { "%{wks.location}/bin/" .. outputdir .. "/" .. projectName }


	links
	{
		projectName,
	}
	CopyLib(projectName)
end

function LinkDependency(depName)
	if not (type( IncludeDir[depName] ) == "string") then
		for i,dir in pairs(IncludeDir[depName]) do
			includedirs { "%{wks.location}/"..dir }
		end
	else
		includedirs{"%{wks.location}/%{IncludeDir."..depName.."}"}
	end
	libdirs {"%{wks.location}/bin/"..outputdir.."/vendor/"..depName.."/"}
	links
	{
		depName,
	}
	CopyDependencyLib(depName)
end

function SetupProject(projectName)
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")
	ProjectFiles(projectName)
	-- everybody needs guid
	includedirs{"%{wks.location}/%{IncludeDir.crossguid}"}
	defines
	{
		"BUILD_"..string.upper(tostring(projectName)).."_DLL",
	}
end
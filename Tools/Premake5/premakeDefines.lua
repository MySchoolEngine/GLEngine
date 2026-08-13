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
		".clang-format",
		"*.natvis",
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

-- Many unrelated projects each copy their own build target into Sandbox/ so it has
-- everything it needs to run. Under a parallel build (-m/-maxCpuCount) several of them
-- can race to write into that shared, not-yet-existing directory at the same time, which
-- intermittently fails with "xcopy ... exited with code 4". robocopy handles concurrent
-- directory creation more gracefully and /R/W give it a few retries on transient failures;
-- its exit codes 0-7 all mean success (see https://learn.microsoft.com/windows-server/administration/windows-commands/robocopy),
-- so we normalize that into a plain 0/1 MSBuild expects.
function CopyToSandbox()
	if _TARGET_OS == "windows" then
		-- robocopy treats "/" as its switch prefix, so unlike the {COPY} xcopy-based
		-- helpers above, this path must be all-backslash or robocopy misparses it.
		-- The trailing "." on each directory arg avoids a classic cmd.exe quoting trap:
		-- a backslash right before a closing quote escapes the quote instead of ending
		-- the path, which silently merges the source and destination arguments together.
		local sandboxDir = "%{wks.location}\\bin\\" .. outputdir .. "\\Sandbox\\."
		postbuildcommands
		{
			("robocopy \"%{cfg.buildtarget.directory}.\" \"" .. sandboxDir .. "\" \"%{cfg.buildtarget.name}\" /R:5 /W:1 /NP /NFL /NDL /NJH /NJS & if %errorlevel% geq 8 (exit /b 1) else (exit /b 0)")
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

function Link(projectName, isStatic)
	isStatic = isStatic or false  -- Default to false (DLL)

	includedirs("%{wks.location}/" .. projectName)

	if isStatic then
		-- Link against static library variant
		local staticProjectName = projectName .. "Static"
		libdirs { "%{wks.location}/bin/" .. outputdir .. "/" .. staticProjectName }
		links { staticProjectName }
		usage "PUBLIC"
			defines { string.upper(projectName) .. "_STATIC_BUILD" }
		-- No need to copy static libs
	else
		-- Link against DLL
		libdirs { "%{wks.location}/bin/" .. outputdir .. "/" .. projectName }
		links { projectName }
		CopyLib(projectName)
	end
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
	filter "action:gmake*"
		disablewarnings "unknown-pragmas"
	filter {}
end
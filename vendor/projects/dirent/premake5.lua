filter "system:windows"
    project "Dirent"
        kind "StaticLib"
        language "C++"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "Off"
        
    	targetdir ("%{wks.location}/bin/" .. outputdir .. "/vendor/%{prj.name}")
        objdir ("%{wks.location}/obj/" .. outputdir .. "/vendor/%{prj.name}")

        includedirs
        {
            "%{wks.location}/%{IncludeDir.dirent}",
        }

    	files
    	{
            "%{wks.location}/%{IncludeDir.dirent}*.h",
        }
            
        filter "configurations:Debug"
            runtime "Debug"
            symbols "on"

        filter "configurations:Release"
            runtime "Release"
            optimize "on"

filter "system:windows"
    project "Dirent"
        kind "StaticLib"
        language "C++"
        
    	targetdir ("../../../bin/" .. outputdir .. "/vendor/%{prj.name}")
        objdir ("../../../obj/" .. outputdir .. "/vendor/%{prj.name}")

        includedirs
        {
            "../../../%{IncludeDir.dirent}",
        }

    	files
    	{
            "../../../%{IncludeDir.dirent}*.h",
        }
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "Off"
            
        filter "configurations:Debug"
            runtime "Debug"
            symbols "on"

        filter "configurations:Release"
            runtime "Release"
            optimize "on"

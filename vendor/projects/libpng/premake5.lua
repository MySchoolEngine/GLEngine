project "libpng"
    kind "StaticLib"
    language "C++"
    
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/vendor/%{prj.name}")

    includedirs
    {
        "%{wks.location}/vendor/libpng/",
        ".",
        "%{wks.location}/vendor/Assimp/Contrib/zlib/",
        "../zlib"
    }

    files
    {
        "%{wks.location}/vendor/libpng/*.h",
        "%{wks.location}/vendor/libpng/*.c",
    }

    links
    {
        "zlib",
    }

    filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "On"

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\"")
        }
        disablewarnings 
        { 
            "4267", --  conversion from 'size_t' to 'unsigned long', possible loss of data
        }

    filter "system:linux"
        pic "On"
        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

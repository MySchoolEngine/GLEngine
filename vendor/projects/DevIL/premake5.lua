project "DevIL-IL"
    kind "SharedLib"
    language "C++"
    
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/vendor/%{prj.name}")

    includedirs
    {
        "%{wks.location}/vendor/DevIL/DevIL/src-IL/include",
        "%{wks.location}/vendor/DevIL/DevIL/include/IL",
        "%{wks.location}/vendor/DevIL/DevIL/include",
        "%{wks.location}/vendor/libpng/",
        "%{wks.location}/vendor/libjpeg/",
        "../libjpeg/",
        "../libpng/",
        ".",
    }

	files
	{
        "%{wks.location}/vendor/DevIL/DevIL/src-IL/include/*.h",
        "./config.h",
        "%{wks.location}/vendor/DevIL/DevIL/include/IL/devil_internal_exports.h",
        "%{wks.location}/vendor/DevIL/DevIL/include/IL/il.h",
        "%{wks.location}/vendor/DevIL/DevIL/src-IL/src/*.cpp",
    }

    defines
    {
        "IL_NO_EXR",
        "IL_NO_WDP",
        "IL_NO_JP2",
        "IL_NO_MNG",
        "IL_NO_LCMS",
        "IL_NO_TIF",
    }

    links
    {
        "libjpeg",
        "libpng",
    }
    
    removedefines
    {
        "_UNICODE", 
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
            "4101", -- sstrcpy
            "5033", -- register keyword
            "4065", -- default but no case label 
            "4756", -- overflow in constant arithmetic
        }
        
    filter "system:linux"
        disablewarnings
        {
            "conversion-null",
            "register",
            "unused-result",
        }
        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

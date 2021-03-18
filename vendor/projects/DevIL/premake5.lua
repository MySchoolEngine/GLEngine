project "DevIL-IL"
    kind "SharedLib"
    language "C++"
    
	targetdir ("../../../bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("../../../obj/" .. outputdir .. "/vendor/%{prj.name}")

    includedirs
    {
        "../../DevIL/DevIL/src-IL/include",
        "../../DevIL/DevIL/include/IL",
        "../../DevIL/DevIL/include",
        "../../libjpeg/",
        "../libjpeg/",
        ".",
    }

	files
	{
        "../../DevIL/DevIL/src-IL/include/*.h",
        "./config.h",
        "../../DevIL/DevIL/include/IL/devil_internal_exports.h",
        "../../DevIL/DevIL/include/IL/il.h",
        "../../DevIL/DevIL/src-IL/src/*.cpp",
    }

    defines
    {
        "IL_NO_EXR",
        "IL_NO_WDP",
        "IL_NO_JP2",
        "IL_NO_PNG",
        "IL_NO_MNG",
        "IL_NO_LCMS",
        "IL_NO_TIF",
    }

    links
    {
        "libjpeg",
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
            ("{COPY} %{cfg.buildtarget.relpath} \"../../../bin/" .. outputdir .. "/Sandbox/\"")
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

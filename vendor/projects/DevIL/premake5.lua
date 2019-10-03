project "DevIL-IL"
    kind "SharedLib"
    language "C++"
    
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")


    includedirs
    {
        "../../DevIL/DevIL/src-IL/include",
        "../../DevIL/DevIL/include/IL",
        "../../DevIL/DevIL/include",
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
        "IL_NO_JPG",
        "IL_NO_PNG",
        "IL_NO_MNG",
        "IL_NO_LCMS",
        "IL_NO_TIF",
    }
    
	filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "On"
        
    filter { "system:windows", "configurations:Release" }
        buildoptions "/MT"

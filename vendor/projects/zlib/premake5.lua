project "zlib"
    kind "SharedLib"
    language "C++"
    
	targetdir ("../../../bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("../../../obj/" .. outputdir .. "/vendor/%{prj.name}")

    includedirs
    {
        "../../Assimp/contrib/zlib",
        ".",
    }

	files
	{
        "../../Assimp/contrib/zlib/crc32.h",
        "../../Assimp/contrib/zlib/deflate.h",
        "../../Assimp/contrib/zlib/gzguts.h",
        "../../Assimp/contrib/zlib/inffast.h",
        "../../Assimp/contrib/zlib/inffixed.h",
        "../../Assimp/contrib/zlib/inflate.h",
        "../../Assimp/contrib/zlib/inftrees.h",
        "../../Assimp/contrib/zlib/trees.h",
        "../../Assimp/contrib/zlib/zutil.h",
        "../../Assimp/contrib/zlib/adler32.c",
        "../../Assimp/contrib/zlib/compress.c",
        "../../Assimp/contrib/zlib/crc32.c",
        "../../Assimp/contrib/zlib/deflate.c",
        "../../Assimp/contrib/zlib/gzclose.c",
        "../../Assimp/contrib/zlib/gzlib.c",
        "../../Assimp/contrib/zlib/gzread.c",
        "../../Assimp/contrib/zlib/gzwrite.c",
        "../../Assimp/contrib/zlib/inflate.c",
        "../../Assimp/contrib/zlib/infback.c",
        "../../Assimp/contrib/zlib/inftrees.c",
        "../../Assimp/contrib/zlib/inffast.c",
        "../../Assimp/contrib/zlib/trees.c",
        "../../Assimp/contrib/zlib/uncompr.c",
        "../../Assimp/contrib/zlib/zutil.c",
        "./zconf.h",
    }

    defines
    {
        "ZLIB_DLL",
    }
    
	filter "system:windows"
        staticruntime "On"

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../../../bin/" .. outputdir .. "/Sandbox/\"")
        }
        defines
        {
            "_CRT_SECURE_NO_DEPRECATE",
            "_CRT_NONSTDC_NO_DEPRECATE",
        }
        buildoptions {
            "/bigobj",
            "/MP",
        }
        files
        {
            "../../Assimp/contrib/zlib/win32/zlib1.rc"
        }
        
    filter "system:not windows"
        defines { 'HAVE_UNISTD_H' }
        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

project "zlib"
    kind "SharedLib"
    language "C++"
    
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/vendor/%{prj.name}")

    includedirs
    {
        "%{wks.location}/vendor/Assimp/contrib/zlib",
        ".",
    }

	files
	{
        "%{wks.location}/vendor/Assimp/contrib/zlib/crc32.h",
        "%{wks.location}/vendor/Assimp/contrib/zlib/deflate.h",
        "%{wks.location}/vendor/Assimp/contrib/zlib/gzguts.h",
        "%{wks.location}/vendor/Assimp/contrib/zlib/inffast.h",
        "%{wks.location}/vendor/Assimp/contrib/zlib/inffixed.h",
        "%{wks.location}/vendor/Assimp/contrib/zlib/inflate.h",
        "%{wks.location}/vendor/Assimp/contrib/zlib/inftrees.h",
        "%{wks.location}/vendor/Assimp/contrib/zlib/trees.h",
        "%{wks.location}/vendor/Assimp/contrib/zlib/zutil.h",
        "%{wks.location}/vendor/Assimp/contrib/zlib/adler32.c",
        "%{wks.location}/vendor/Assimp/contrib/zlib/compress.c",
        "%{wks.location}/vendor/Assimp/contrib/zlib/crc32.c",
        "%{wks.location}/vendor/Assimp/contrib/zlib/deflate.c",
        "%{wks.location}/vendor/Assimp/contrib/zlib/gzclose.c",
        "%{wks.location}/vendor/Assimp/contrib/zlib/gzlib.c",
        "%{wks.location}/vendor/Assimp/contrib/zlib/gzread.c",
        "%{wks.location}/vendor/Assimp/contrib/zlib/gzwrite.c",
        "%{wks.location}/vendor/Assimp/contrib/zlib/inflate.c",
        "%{wks.location}/vendor/Assimp/contrib/zlib/infback.c",
        "%{wks.location}/vendor/Assimp/contrib/zlib/inftrees.c",
        "%{wks.location}/vendor/Assimp/contrib/zlib/inffast.c",
        "%{wks.location}/vendor/Assimp/contrib/zlib/trees.c",
        "%{wks.location}/vendor/Assimp/contrib/zlib/uncompr.c",
        "%{wks.location}/vendor/Assimp/contrib/zlib/zutil.c",
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
            ("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\"")
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
            "%{wks.location}/vendor/Assimp/contrib/zlib/win32/zlib1.rc"
        }
        
    filter "system:not windows"
        defines { 'HAVE_UNISTD_H' }
        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

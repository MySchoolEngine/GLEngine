project "libjpeg"
    kind "StaticLib"
    language "C++"
    
	targetdir ("../../../bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("../../../obj/" .. outputdir .. "/vendor/%{prj.name}")

    includedirs
    {
        "../../libjpeg/",
        ".",
    }

	files
	{
        "../../libjpeg/jerror.h",
        "../../libjpeg/jmorecfg.h",
        "../../libjpeg/jpeglib.h",
        "../../libjpeg/jmemnobs.c",
        "../../libjpeg/jaricom.c",
        "../../libjpeg/jcapimin.c",
        "../../libjpeg/jcapistd.c",
        "../../libjpeg/jcarith.c",
        "../../libjpeg/jccoefct.c",
        "../../libjpeg/jccolor.c",
        "../../libjpeg/jcdctmgr.c",
        "../../libjpeg/jchuff.c",
        "../../libjpeg/jcinit.c",
        "../../libjpeg/jcmainct.c",
        "../../libjpeg/jcmarker.c",
        "../../libjpeg/jcmaster.c",
        "../../libjpeg/jcomapi.c",
        "../../libjpeg/jcparam.c",
        "../../libjpeg/jcprepct.c",
        "../../libjpeg/jcsample.c",
        "../../libjpeg/jctrans.c",
        "../../libjpeg/jdapimin.c",
        "../../libjpeg/jdapistd.c",
        "../../libjpeg/jdarith.c",
        "../../libjpeg/jdatadst.c",
        "../../libjpeg/jdatasrc.c",
        "../../libjpeg/jdcoefct.c",
        "../../libjpeg/jdcolor.c",
        "../../libjpeg/jddctmgr.c",
        "../../libjpeg/jdhuff.c",
        "../../libjpeg/jdinput.c",
        "../../libjpeg/jdmainct.c",
        "../../libjpeg/jdmarker.c",
        "../../libjpeg/jdmaster.c",
        "../../libjpeg/jdmerge.c",
        "../../libjpeg/jdpostct.c",
        "../../libjpeg/jdsample.c",
        "../../libjpeg/jdtrans.c",
        "../../libjpeg/jerror.c",
        "../../libjpeg/jfdctflt.c",
        "../../libjpeg/jfdctfst.c",
        "../../libjpeg/jfdctint.c",
        "../../libjpeg/jidctflt.c",
        "../../libjpeg/jidctfst.c",
        "../../libjpeg/jidctint.c",
        "../../libjpeg/jquant1.c",
        "../../libjpeg/jquant2.c",
        "../../libjpeg/jutils.c",
        "../../libjpeg/jmemmgr.c",
        "../../libjpeg/cderror.h",
        "../../libjpeg/cdjpeg.h",
        "../../libjpeg/jdct.h",
        "../../libjpeg/jinclude.h",
        "../../libjpeg/jmemsys.h",
        "../../libjpeg/jpegint.h",
        "../../libjpeg/jversion.h",
        "../../libjpeg/transupp.h",
        "../../libjpeg/jconfig.h",
    }

	filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "On"

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../../../bin/" .. outputdir .. "/Sandbox/\"")
        }

    filter "system:linux"
        pic "On"
        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

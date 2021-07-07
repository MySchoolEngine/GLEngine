project "libjpeg"
    kind "StaticLib"
    language "C++"
    
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/vendor/%{prj.name}")

    includedirs
    {
        "%{wks.location}/vendor/libjpeg/",
        ".",
    }

	files
	{
        "%{wks.location}/vendor/libjpeg/jerror.h",
        "%{wks.location}/vendor/libjpeg/jmorecfg.h",
        "%{wks.location}/vendor/libjpeg/jpeglib.h",
        "%{wks.location}/vendor/libjpeg/jmemnobs.c",
        "%{wks.location}/vendor/libjpeg/jaricom.c",
        "%{wks.location}/vendor/libjpeg/jcapimin.c",
        "%{wks.location}/vendor/libjpeg/jcapistd.c",
        "%{wks.location}/vendor/libjpeg/jcarith.c",
        "%{wks.location}/vendor/libjpeg/jccoefct.c",
        "%{wks.location}/vendor/libjpeg/jccolor.c",
        "%{wks.location}/vendor/libjpeg/jcdctmgr.c",
        "%{wks.location}/vendor/libjpeg/jchuff.c",
        "%{wks.location}/vendor/libjpeg/jcinit.c",
        "%{wks.location}/vendor/libjpeg/jcmainct.c",
        "%{wks.location}/vendor/libjpeg/jcmarker.c",
        "%{wks.location}/vendor/libjpeg/jcmaster.c",
        "%{wks.location}/vendor/libjpeg/jcomapi.c",
        "%{wks.location}/vendor/libjpeg/jcparam.c",
        "%{wks.location}/vendor/libjpeg/jcprepct.c",
        "%{wks.location}/vendor/libjpeg/jcsample.c",
        "%{wks.location}/vendor/libjpeg/jctrans.c",
        "%{wks.location}/vendor/libjpeg/jdapimin.c",
        "%{wks.location}/vendor/libjpeg/jdapistd.c",
        "%{wks.location}/vendor/libjpeg/jdarith.c",
        "%{wks.location}/vendor/libjpeg/jdatadst.c",
        "%{wks.location}/vendor/libjpeg/jdatasrc.c",
        "%{wks.location}/vendor/libjpeg/jdcoefct.c",
        "%{wks.location}/vendor/libjpeg/jdcolor.c",
        "%{wks.location}/vendor/libjpeg/jddctmgr.c",
        "%{wks.location}/vendor/libjpeg/jdhuff.c",
        "%{wks.location}/vendor/libjpeg/jdinput.c",
        "%{wks.location}/vendor/libjpeg/jdmainct.c",
        "%{wks.location}/vendor/libjpeg/jdmarker.c",
        "%{wks.location}/vendor/libjpeg/jdmaster.c",
        "%{wks.location}/vendor/libjpeg/jdmerge.c",
        "%{wks.location}/vendor/libjpeg/jdpostct.c",
        "%{wks.location}/vendor/libjpeg/jdsample.c",
        "%{wks.location}/vendor/libjpeg/jdtrans.c",
        "%{wks.location}/vendor/libjpeg/jerror.c",
        "%{wks.location}/vendor/libjpeg/jfdctflt.c",
        "%{wks.location}/vendor/libjpeg/jfdctfst.c",
        "%{wks.location}/vendor/libjpeg/jfdctint.c",
        "%{wks.location}/vendor/libjpeg/jidctflt.c",
        "%{wks.location}/vendor/libjpeg/jidctfst.c",
        "%{wks.location}/vendor/libjpeg/jidctint.c",
        "%{wks.location}/vendor/libjpeg/jquant1.c",
        "%{wks.location}/vendor/libjpeg/jquant2.c",
        "%{wks.location}/vendor/libjpeg/jutils.c",
        "%{wks.location}/vendor/libjpeg/jmemmgr.c",
        "%{wks.location}/vendor/libjpeg/cderror.h",
        "%{wks.location}/vendor/libjpeg/cdjpeg.h",
        "%{wks.location}/vendor/libjpeg/jdct.h",
        "%{wks.location}/vendor/libjpeg/jinclude.h",
        "%{wks.location}/vendor/libjpeg/jmemsys.h",
        "%{wks.location}/vendor/libjpeg/jpegint.h",
        "%{wks.location}/vendor/libjpeg/jversion.h",
        "%{wks.location}/vendor/libjpeg/transupp.h",
        "%{wks.location}/vendor/libjpeg/jconfig.h",
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

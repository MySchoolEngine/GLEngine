project "Assimp"
    kind "SharedLib"
    language "C++"
    
	targetdir ("../../../bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("../../../obj/" .. outputdir .. "/vendor/%{prj.name}")

    ContribDir = "../../Assimp/contrib"
    UnzipIncludeDir = ContribDir.."/unzip"
    OpenDDLParserDir = ContribDir.."/openddlparser"
    Poly2Tri = ContribDir.."/poly2tri/poly2tri"
    zipDir = ContribDir.."/zip"
    includedirs
    {
        ContribDir.."/zlib",
        ContribDir.."/irrXML",
        UnzipIncludeDir,
        OpenDDLParserDir.."/include",
        ContribDir.."/rapidjson/include",
        "../../Assimp/", -- somewhere in code <contrib/zlib/zlib.h>
        ContribDir,
        "../../Assimp/include/",
        "../zlib",
        ".",
    }

	files
	{
        "../../Assimp/include/**.hpp",
        "../../Assimp/include/**.h",
        "../../Assimp/code/*.h",
        "../../Assimp/code/*.cpp",
        "../../Assimp/code/Importer/StepFile/*.h",
        "../../Assimp/code/Importer/StepFile/*.cpp",
        "../../Assimp/code/Importer/STEPParser/*.h",
        "../../Assimp/code/Importer/STEPParser/*.cpp",

        "../../Assimp/code/Importer/IFC/IFCUtil.*",
        "../../Assimp/code/Importer/IFC/IFCReaderGen_2x3.h",
        "../../Assimp/code/Importer/IFC/IFCReaderGen1_2x3.cpp",
        "../../Assimp/code/Importer/IFC/IFCReaderGen2_2x3.cpp",
        "../../Assimp/code/Importer/IFC/IFCLoader.*",
        "../../Assimp/code/Importer/IFC/IFCProfile.cpp",
        "../../Assimp/code/Importer/IFC/IFCOpenings.cpp",
        "../../Assimp/code/Importer/IFC/IFCMaterial.cpp",
        "../../Assimp/code/Importer/IFC/IFCGeometry.cpp",
        "../../Assimp/code/Importer/IFC/IFCCurve.cpp",
        "../../Assimp/code/Importer/IFC/IFCBoolean.cpp",

        ContribDir.."/clipper/clipper.hpp",
        ContribDir.."/clipper/clipper.cpp",

        UnzipIncludeDir.."/crypt.h",
        UnzipIncludeDir.."/ioapi.c",
        UnzipIncludeDir.."/ioapi.h",
        UnzipIncludeDir.."/unzip.h",
        UnzipIncludeDir.."/unzip.c",
        OpenDDLParserDir.."/code/*.cpp",
        OpenDDLParserDir.."/include/openddlparser/*.h",
        Poly2Tri.."/sweep/advancing_front.*",
        Poly2Tri.."/sweep/cdt.*",
        Poly2Tri.."/sweep/sweep.*",
        Poly2Tri.."/sweep/sweep_context.*",
        Poly2Tri.."/common/utils.h",
        Poly2Tri.."/common/shapes.*",
        zipDir.."/src/zip.h",
        zipDir.."/src/zip.c",
        zipDir.."/src/miniz.h",
        "revision.h",
        "./assimp/config.h"
    }

    defines
    {
        "ASSIMP_BUILD_NO_C4D_IMPORTER",
        "ASSIMP_BUILD_DLL_EXPORT",
        "OPENDDLPARSER_BUILD",
    }

    links
    {
        "zlib",
        "irrXML",
    }
    
	filter "system:windows"
        staticruntime "On"

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../../../bin/" .. outputdir .. "/Sandbox/\"")
        }
        defines
        {
            "WINDOWS",
        }
        buildoptions {
            "/bigobj",
            "/MP",
        }
        disablewarnings 
        { 
            "4996", -- The std::iterator class template (used as a base class to provide typedefs) is deprecated in C++17.
            "4065", -- switch statement contains 'default' but no 'case' labels
        }
        filter  "configurations:Release"
            defines
            {
                "NDEBUG",
            }

        filter "configurations:Debug"
            defines
            {
                "_CRT_SECURE_NO_WARNINGS",
                "_CRT_SECURE_NO_WARNINGS",
                "WIN32_LEAN_AND_MEAN",
            }
        
    filter "system:linux"
        pic "On"
        disablewarnings 
        {
            "stringop-overflow",
            "unused-result",
        }
        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

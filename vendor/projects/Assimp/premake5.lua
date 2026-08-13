project "Assimp"
    kind "SharedLib"
    language "C++"

    LinkDependency("pugixml")
    
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/vendor/%{prj.name}")

    ContribDir = "../../Assimp/contrib"
    UnzipIncludeDir = ContribDir.."/unzip"
    OpenDDLParserDir = ContribDir.."/openddlparser"
    Poly2Tri = ContribDir.."/poly2tri/poly2tri"
    zipDir = ContribDir.."/zip"
    includedirs
    {
        ContribDir.."/zlib",
        ContribDir.."/utf8cpp/source",
        ContribDir.."/irrXML",
        UnzipIncludeDir,
        OpenDDLParserDir.."/include",
        ContribDir.."/rapidjson/include",
        "../../Assimp/", -- somewhere in code <contrib/zlib/zlib.h>
        ContribDir,
        "../../Assimp/include/",
        "../zlib",
        "../../Assimp/code",
        ".",
    }

	files
	{
        "../../Assimp/include/**.hpp",
        "../../Assimp/include/**.h",
        "../../Assimp/code/**.h",
        "../../Assimp/code/**.cpp",

        ContribDir.."/clipper/clipper.hpp",
        ContribDir.."/clipper/clipper.cpp",

        UnzipIncludeDir.."/crypt.h",
        UnzipIncludeDir.."/ioapi.h",
        UnzipIncludeDir.."/ioapi.c",
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
    removefiles
    {
         "../../Assimp/code/AssetLib/MDL/HalfLife/**.h",  -- no Half-Life please
         "../../Assimp/code/AssetLib/MDL/HalfLife/**.cpp",-- no Half-Life please
         "../../Assimp/code/AssetLib/USD/**.h",           -- no USD/tinyusdz
         "../../Assimp/code/AssetLib/USD/**.cpp",         -- no USD/tinyusdz
    }


    defines
    {
        "ASSIMP_BUILD_NO_C4D_IMPORTER",
        -- 3DS
        "ASSIMP_BUILD_NO_3DS_IMPORTER", -- not used right now, faster build
        "ASSIMP_BUILD_NO_3DS_EXPORTER", -- not used right now, faster build
        -- 3MF
        "ASSIMP_BUILD_NO_3MF_IMPORTER", -- not used right now, faster build
        "ASSIMP_BUILD_NO_3DS_EXPORTER", -- not used right now, faster build
        -- AC
        "ASSIMP_BUILD_NO_AC_IMPORTER", -- not used right now, faster build
        -- AMF
        "ASSIMP_BUILD_NO_AMF_IMPORTER", -- not used right now, faster build
        -- ASE
        "ASSIMP_BUILD_NO_ASE_IMPORTER", -- not used right now, faster build
        -- MDL
        "ASSIMP_BUILD_NO_MDL_IMPORTER", -- not used right now, faster build
        -- HMP
        "ASSIMP_BUILD_NO_HMP_IMPORTER", -- gets broken after removing Half-Life 1 loader
        "ASSIMP_BUILD_NO_IFC_IMPORTER", -- not used right now, faster build
        "ASSIMP_BUILD_NO_ASSJSON_EXPORTER", -- not used right now, build does not work

        -- M3D
        "ASSIMP_BUILD_NO_M3D_IMPORTER", -- not used right now, build does not work
        "ASSIMP_BUILD_NO_M3D_EXPORTER", -- not used right now, build does not work

        -- VRML
        "ASSIMP_BUILD_NO_VRML_IMPORTER", -- not used right now, build does not work

        -- USD
        "ASSIMP_BUILD_NO_USD_IMPORTER",  -- requires tinyusdz, not needed

        "ASSIMP_BUILD_DLL_EXPORT",
        "OPENDDLPARSER_BUILD",

        "RAPIDJSON_HAS_STDSTRING", --required in GLTF2
    }

    links
    {
        "zlib",
        "pugixml",
    }
    
	filter "system:windows"
        staticruntime "off"

        CopyToSandbox()
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
            "4244", -- conversion from 'double' to 'ai_real' (assimp builds ai_real as float by default), possible loss of data
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

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

project "irrXML"
    kind "StaticLib"
    language "C++"
    
	targetdir ("../../../bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("../../../obj/" .. outputdir .. "/vendor/%{prj.name}")

    includedirs
    {
        "../../Assimp/contrib/irrXML",
        "../../Assimp/include",
        "../Assimp", -- config.h
    }

	files
	{
		"../../Assimp/contrib/irrXML/CXMLReaderImpl.h",
		"../../Assimp/contrib/irrXML/heapsort.h",
		"../../Assimp/contrib/irrXML/irrArray.h",
		"../../Assimp/contrib/irrXML/irrString.h",
		"../../Assimp/contrib/irrXML/irrTypes.h",
		"../../Assimp/contrib/irrXML/irrXML.cpp",
		"../../Assimp/contrib/irrXML/irrXML.h",
    }
    
	filter "system:windows"
        staticruntime "On"

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../../../bin/" .. outputdir .. "/Sandbox/\"")
        }
        defines
        {
            "_SCL_SECURE_NO_WARNINGS",
            "_CRT_SECURE_NO_WARNINGS",
        }

    filter "system:linux"
        pic "On"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

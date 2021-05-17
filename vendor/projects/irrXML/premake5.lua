project "irrXML"
    kind "StaticLib"
    language "C++"
    
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/vendor/%{prj.name}")

    includedirs
    {
        "%{wks.location}/vendor/Assimp/contrib/irrXML",
        "%{wks.location}/vendor/Assimp/include",
        "../Assimp", -- config.h
    }

	files
	{
		"%{wks.location}/vendor/Assimp/contrib/irrXML/CXMLReaderImpl.h",
		"%{wks.location}/vendor/Assimp/contrib/irrXML/heapsort.h",
		"%{wks.location}/vendor/Assimp/contrib/irrXML/irrArray.h",
		"%{wks.location}/vendor/Assimp/contrib/irrXML/irrString.h",
		"%{wks.location}/vendor/Assimp/contrib/irrXML/irrTypes.h",
		"%{wks.location}/vendor/Assimp/contrib/irrXML/irrXML.cpp",
		"%{wks.location}/vendor/Assimp/contrib/irrXML/irrXML.h",
    }
    
	filter "system:windows"
        staticruntime "On"

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\"")
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

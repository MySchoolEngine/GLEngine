project "RTTR"
    kind "SharedLib"
    language "C++"
    optimize "off"
    cppdialect "C++17"
    staticruntime "off"
    
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/vendor/%{prj.name}")

    libPath = "%{wks.location}/vendor/RTTR/src/"

    -- pchheader (libPath.."detail/base/pch.h")

    includedirs
    {
        libPath,
        ".",
        --"3rd_party/rapidjson-1.1.0",
    }

	files
	{
        libPath.."rttr/**",
        libPath.."rttr/**.cpp",
    }

    excludes { libPath.."/" }

    defines
    {
        "RTTR_DLL",
        "RTTR_DLL_EXPORTS",
    }

    filter "configurations:Debug"
        runtime "Debug"
    filter "configurations:Release"
        runtime "Release"

	filter "system:windows"

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../../../bin/" .. outputdir .. "/Sandbox/\"")
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

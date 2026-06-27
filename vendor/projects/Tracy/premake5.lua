project "Tracy"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir    ("%{wks.location}/obj/" .. outputdir .. "/vendor/%{prj.name}")

    files
    {
        "%{wks.location}/vendor/tracy/public/TracyClient.cpp",
    }

    includedirs
    {
        "%{wks.location}/vendor/tracy/public",
    }

    filter "system:windows"
        systemversion "latest"
        links { "ws2_32", "dbghelp", "advapi32" }
        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\"")
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        defines { "TRACY_ENABLE", "TRACY_EXPORTS" }

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:ASAN"
        runtime "Debug"
        symbols "On"

    filter {}
    usage "PUBLIC"
        filter "configurations:Debug"
            defines { "TRACY_ENABLE", "TRACY_IMPORTS" }
        filter {}

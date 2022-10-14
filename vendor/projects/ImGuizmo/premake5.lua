project "ImGuizmo"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/vendor/%{prj.name}")

    includedirs
    {
        "%{wks.location}/vendor/ImGuizmo",
        "%{wks.location}/%{IncludeDir.ImGui}",
        ".",
    }

	files
	{
        "%{wks.location}/vendor/ImGuizmo/*.h",
        "%{wks.location}/vendor/ImGuizmo/*.cpp",
    }
    
    filter "system:linux"
        pic "On"
        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

project "ImGuizmo"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/vendor/%{prj.name}")

    includedirs
    {
        "%{wks.location}/%{IncludeDir.ImGuizmo}",
        "%{wks.location}/%{IncludeDir.ImGui}",
        ".",
    }

	files
	{
        "%{wks.location}/%{IncludeDir.ImGuizmo}/*.h",
        "%{wks.location}/%{IncludeDir.ImGuizmo}/*.cpp",
    }
    
    filter "system:linux"
        pic "On"

    filter "system:windows"
        disablewarnings
        {
            "4244", -- 'argument': conversion from 'float' to 'ImDrawFlags', possible loss of data (bit-flag param passed as float default)
            "4005", -- 'IMGUI_DEFINE_MATH_OPERATORS': macro redefinition (also defined workspace-wide; GraphEditor.cpp redefines it locally)
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

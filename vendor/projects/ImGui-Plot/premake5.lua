project "ImGuiPlot"
    kind "StaticLib"
    language "C++"
    staticruntime "off"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "../../imgui-plot/include/imgui_plot.h",
        "../../imgui-plot/src/imgui_plot.cpp"
    }

    includedirs
    {
        "../../../%{IncludeDir.ImGui}",
        "../../../%{IncludeDir.ImGuiPlot}",
    }
    
    filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"


    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

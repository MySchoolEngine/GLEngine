project "ImGuiFileDialog"
    kind "StaticLib"
    language "C++"
    
	targetdir ("../../../bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("../../../obj/" .. outputdir .. "/vendor/%{prj.name}")

    includedirs
    {
        "../../ImGuiFileDialog/ImGuiFileDialog",
        "../../../%{IncludeDir.ImGui}",
        "../../../%{IncludeDir.dirent}",
        ".",
    }

	files
	{
        "../../ImGuiFileDialog/ImGuiFileDialog/*.h",
        "../../ImGuiFileDialog/ImGuiFileDialog/*.cpp",
    }
    
	filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "Off"
        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

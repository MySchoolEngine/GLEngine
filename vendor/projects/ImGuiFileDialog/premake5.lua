project "ImGuiFileDialog"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    
	targetdir ("../../../bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("../../../obj/" .. outputdir .. "/vendor/%{prj.name}")

    includedirs
    {
        "../../ImGuiFileDialog/ImGuiFileDialog",
        "../../../%{IncludeDir.ImGui}",
        ".",
    }

	files
	{
        "../../ImGuiFileDialog/ImGuiFileDialog/*.h",
        "../../ImGuiFileDialog/ImGuiFileDialog/*.cpp",
    }
    
	filter "system:windows"
        systemversion "latest"
        staticruntime "Off"
        includedirs
        {
            "../../../%{IncludeDir.dirent}",
        }
        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

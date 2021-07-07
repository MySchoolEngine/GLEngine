project "ImGuiFileDialog"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/vendor/%{prj.name}")

    includedirs
    {
        "%{wks.location}/vendor/ImGuiFileDialog/ImGuiFileDialog",
        "%{wks.location}/%{IncludeDir.ImGui}",
        ".",
    }

	files
	{
        "%{wks.location}/vendor/ImGuiFileDialog/ImGuiFileDialog/*.h",
        "%{wks.location}/vendor/ImGuiFileDialog/ImGuiFileDialog/*.cpp",
    }
    
	filter "system:windows"
        systemversion "latest"
        staticruntime "Off"
        includedirs
        {
            "%{wks.location}/%{IncludeDir.dirent}",
        }

    filter "system:linux"
        pic "On"
        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

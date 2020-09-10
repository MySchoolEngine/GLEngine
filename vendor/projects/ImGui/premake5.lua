project "ImGui"
    kind "StaticLib"
    language "C++"
    
	targetdir ("../../../bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("../../../obj/" .. outputdir .. "/vendor/%{prj.name}")

	files
	{
        "../../ImGui/imconfig.h",
        "../../ImGui/imgui.h",
        "../../ImGui/imgui.cpp",
        "../../ImGui/imgui_draw.cpp",
        "../../ImGui/imgui_internal.h",
        "../../ImGui/imgui_widgets.cpp",
        "../../ImGui/imstb_rectpack.h",
        "../../ImGui/imstb_textedit.h",
        "../../ImGui/imstb_truetype.h",
        "../../ImGui/imgui_demo.cpp"
    }
    
	filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "On"
        
    filter { "system:windows", "configurations:Release" }
        buildoptions "/MT"

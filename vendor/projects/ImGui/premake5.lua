project "ImGui"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"
    
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
        "../../ImGui/imgui_demo.cpp",
    }

    includedirs
    {
        "../../ImGui",
        "../../../%{IncludeDir.GLFW}",  
        "../../../%{IncludeDir.Glad}",
    }
    defines
    {
        "IMGUI_IMPL_OPENGL_LOADER_GLAD",
        "IMGUI_IMPL_API="
    }
    links
    {
        "GLFW",
        "Glad",
    }
	filter "system:windows"
        systemversion "latest"
        defines
        {
            "IMGUI_API=__declspec(dllexport)",
        }
        
        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../../../bin/" .. outputdir .. "/Sandbox/\"")
        }
    filter { "system:windows", "configurations:Release" }
        buildoptions "/MT"
        
    filter "system:linux"
        defines
        {
            "IMGUI_API=__attribute__((dllexport))",
        }

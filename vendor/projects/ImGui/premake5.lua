project "ImGui"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"
    
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/vendor/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/vendor/%{prj.name}")

    LinkDependency("GLFW")
    LinkDependency("Glad")

	files
	{
        "%{wks.location}/vendor/ImGui/imconfig.h",
        "%{wks.location}/vendor/ImGui/imgui.h",
        "%{wks.location}/vendor/ImGui/imgui.cpp",
        "%{wks.location}/vendor/ImGui/imgui_draw.cpp",
        "%{wks.location}/vendor/ImGui/imgui_internal.h",
        "%{wks.location}/vendor/ImGui/imgui_widgets.cpp",
        "%{wks.location}/vendor/ImGui/imstb_rectpack.h",
        "%{wks.location}/vendor/ImGui/imstb_textedit.h",
        "%{wks.location}/vendor/ImGui/imstb_truetype.h",
        "%{wks.location}/vendor/ImGui/imgui_demo.cpp",
    }

    includedirs
    {
        "%{wks.location}/%{IncludeDir.ImGui}",
    }
    defines
    {
        "IMGUI_IMPL_OPENGL_LOADER_GLAD",
        "IMGUI_IMPL_API="
    }
	filter "system:windows"
        systemversion "latest"
        defines
        {
            "IMGUI_API=__declspec(dllexport)",
        }
        
        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\"")
        }
    filter { "system:windows", "configurations:Release" }
        buildoptions "/MT"
        
    filter "system:linux"
        defines
        {
            "IMGUI_API=",
        }

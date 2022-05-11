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
        "%{wks.location}/"..IncludeDir["ImGui"].."/imconfig.h",
        "%{wks.location}/"..IncludeDir["ImGui"].."/imgui.h",
        "%{wks.location}/"..IncludeDir["ImGui"].."/imgui.cpp",
        "%{wks.location}/"..IncludeDir["ImGui"].."/imgui_draw.cpp",
        "%{wks.location}/"..IncludeDir["ImGui"].."/imgui_internal.h",
        "%{wks.location}/"..IncludeDir["ImGui"].."/imgui_widgets.cpp",
        "%{wks.location}/"..IncludeDir["ImGui"].."/imstb_rectpack.h",
        "%{wks.location}/"..IncludeDir["ImGui"].."/imstb_textedit.h",
        "%{wks.location}/"..IncludeDir["ImGui"].."/imstb_truetype.h",
        "%{wks.location}/"..IncludeDir["ImGui"].."/imgui_demo.cpp",

        "%{wks.location}/"..IncludeDir["ImGui"].."/misc/cpp/imgui_stdlib.h",
        "%{wks.location}/"..IncludeDir["ImGui"].."/misc/cpp/imgui_stdlib.cpp",
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

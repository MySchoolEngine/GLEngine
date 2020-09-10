#include <GLRendererStdafx.h>

#include <GLRenderer/ImGui/GLImGUILayer.h>

#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>

namespace GLEngine::GLRenderer {

//=================================================================================
C_GLImGUILayer::C_GLImGUILayer(GUID window)
	: GUI::C_ImGuiLayer(window)
{

}

//=================================================================================
void C_GLImGUILayer::OnAttach()
{
	GUI::C_ImGuiLayer::OnAttach();
	ImGui_ImplOpenGL3_Init("#version 410");
}

//=================================================================================
void C_GLImGUILayer::OnDetach()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown(); 
	GUI::C_ImGuiLayer::OnDetach();
}

//=================================================================================
void C_GLImGUILayer::FrameBegin()
{
	ImGuiIO& io = ::ImGui::GetIO();

	float time = (float)glfwGetTime();
	io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
	m_Time = time;

	ImGui_ImplOpenGL3_NewFrame();

	GUI::C_ImGuiLayer::FrameBegin();
}

//=================================================================================
void C_GLImGUILayer::FrameEnd()
{
	GUI::C_ImGuiLayer::FrameEnd();
	ImGui_ImplOpenGL3_RenderDrawData(GetRenderData());
}

}
#include <GLRendererStdafx.h>

#include <GLRenderer/ImGui/ImGuiLayer.h>

#include <GLRenderer/ImGui/ImGUIImplOpengl3.h>

#include <Core/Application.h>
#include <Core/IWindowManager.h>
#include <Core/IWindow.h>

#include <Core/EventSystem/EventDispatcher.h>
#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/Event/MouseEvents.h>

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>



namespace GLEngine {
namespace GLRenderer {
namespace ImGui {


//=================================================================================
C_ImGuiLayer::C_ImGuiLayer(GUID window)
	: Core::C_Layer("ImGui")
	, m_Window(window)
{

}

//=================================================================================
C_ImGuiLayer::~C_ImGuiLayer()
{

}

//=================================================================================
void C_ImGuiLayer::OnAttach()
{
	::ImGui::CreateContext();
	::ImGui::StyleColorsDark();

	::ImGuiIO& io = ::ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

	// TEMPORARY: should eventually use Hazel key codes
	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

	ImGui_ImplOpenGL3_Init("#version 410");
}

//=================================================================================
void C_ImGuiLayer::OnDetach()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	::ImGui::DestroyContext();
}

//=================================================================================
void C_ImGuiLayer::OnUpdate()
{
	m_GUIMgr.OnUpdate();
}

//=================================================================================
void C_ImGuiLayer::FrameBegin()
{
	auto window = Core::C_Application::Get().GetWndMgr().GetWindow(m_Window);
	ImGuiIO& io = ::ImGui::GetIO();
	io.DisplaySize = ImVec2(static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));

	float time = (float)glfwGetTime();
	io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
	m_Time = time;

	ImGui_ImplOpenGL3_NewFrame();
	::ImGui::NewFrame();
}

//=================================================================================
void C_ImGuiLayer::FrameEnd()
{
	::ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
}

//=================================================================================
void C_ImGuiLayer::OnEvent(Core::I_Event& event)
{
	Core::C_EventDispatcher d(event);
	d.Dispatch<Core::C_KeyPressedEvent>(std::bind(&C_ImGuiLayer::OnKeyPressed, this, std::placeholders::_1));
	d.Dispatch<Core::C_KeyReleasedEvent>(std::bind(&C_ImGuiLayer::OnKeyReleased, this, std::placeholders::_1));
	d.Dispatch<Core::C_TextInputEvent>(std::bind(&C_ImGuiLayer::OnTextEvent, this, std::placeholders::_1));
	d.Dispatch<Core::C_MouseButtonPressed>(std::bind(&C_ImGuiLayer::OnMouseButtonPressedEvent, this, std::placeholders::_1));
	d.Dispatch<Core::C_MouseButtonReleased>(std::bind(&C_ImGuiLayer::OnMouseButtonReleasedEvent, this, std::placeholders::_1));
	d.Dispatch<Core::C_MouseScrollEvent>(std::bind(&C_ImGuiLayer::OnMouseScrolledEvent, this, std::placeholders::_1));
	d.Dispatch<Core::C_MouseMoved>(std::bind(&C_ImGuiLayer::OnMouseMoved, this, std::placeholders::_1));
}

//=================================================================================
bool C_ImGuiLayer::CapturingMouse() const
{
	ImGuiIO& io = ::ImGui::GetIO();
	return io.WantCaptureMouse;
}

//=================================================================================
GUI::C_GUIManager& C_ImGuiLayer::GetGUIMgr()
{
	return m_GUIMgr;
}

//=================================================================================
bool C_ImGuiLayer::OnKeyPressed(Core::C_KeyPressedEvent& event)
{
	ImGuiIO& io = ::ImGui::GetIO();
	io.KeysDown[event.GetKeyCode()] = true;
	
	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
	return io.WantCaptureKeyboard;
}

//=================================================================================
bool C_ImGuiLayer::OnKeyReleased(Core::C_KeyReleasedEvent& event)
{
	ImGuiIO& io = ::ImGui::GetIO();
	io.KeysDown[event.GetKeyCode()] = false;

	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
	return io.WantCaptureKeyboard;
}

//=================================================================================
bool C_ImGuiLayer::OnTextEvent(Core::C_TextInputEvent& event)
{
	ImGuiIO& io = ::ImGui::GetIO();
	if (event.GetCodePoint() > 0 && event.GetCodePoint() < 0x10000)
		io.AddInputCharacter((unsigned short)event.GetCodePoint());

	return io.WantCaptureKeyboard;
}

//=================================================================================
bool C_ImGuiLayer::OnMouseButtonPressedEvent(Core::C_MouseButtonPressed& e)
{
	ImGuiIO& io = ::ImGui::GetIO();
	io.MouseDown[e.GetMouseButton()] = true;

	return CapturingMouse();
}

//=================================================================================
bool C_ImGuiLayer::OnMouseButtonReleasedEvent(Core::C_MouseButtonReleased& e)
{
	ImGuiIO& io = ::ImGui::GetIO();
	io.MouseDown[e.GetMouseButton()] = false;

	return CapturingMouse();
}

//=================================================================================
bool C_ImGuiLayer::OnMouseScrolledEvent(Core::C_MouseScrollEvent& e)
{
	ImGuiIO& io = ::ImGui::GetIO();
	io.MouseWheelH += static_cast<float>(e.GetXOffset());
	io.MouseWheel += static_cast<float>(e.GetYOffset());

	return CapturingMouse();
}

//=================================================================================
bool C_ImGuiLayer::OnMouseMoved(Core::C_MouseMoved& e)
{
	ImGuiIO& io = ::ImGui::GetIO();
	io.MousePos = ImVec2(e.GetPosX(), e.GetPosY());

	return CapturingMouse();
}

}}}
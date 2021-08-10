#include <GUIStdafx.h>

#include <GUI/ImGuiLayer.h>

#include <Core/Application.h>
#include <Core/EventSystem/Event/AppEvent.h>
#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/Event/MouseEvents.h>
#include <Core/EventSystem/EventDispatcher.h>
#include <Core/IWindow.h>
#include <Core/IWindowManager.h>
#include <Core/Input.h>

#include <GLFW/glfw3.h>
#include <imgui.h>

namespace GLEngine::GUI {

//=================================================================================
C_ImGuiLayer::C_ImGuiLayer(GUID window)
	: Core::C_Layer("ImGui")
	, m_Window(window)
	, m_Time(0.0f)
{
}

//=================================================================================
C_ImGuiLayer::~C_ImGuiLayer() = default;

//=================================================================================
void C_ImGuiLayer::OnAttach()
{
	::ImGui::CreateContext();
	::ImGui::StyleColorsDark();

	::ImGuiIO& io = ::ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

	io.KeyMap[ImGuiKey_Tab]		   = GLFW_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow]  = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow]	   = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow]  = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp]	   = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown]   = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home]	   = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End]		   = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Insert]	   = GLFW_KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete]	   = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace]  = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space]	   = GLFW_KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter]	   = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape]	   = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A]		   = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C]		   = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V]		   = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X]		   = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y]		   = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z]		   = GLFW_KEY_Z;

	io.FontDefault = io.Fonts->AddFontFromFileTTF("Fonts/Roboto/Roboto-Regular.ttf", 18.0f);
	// io.Fonts->AddFontFromFileTTF("Fonts/material-design-icons/font/MaterialIcons-Regular.ttf", 18.0f);

	auto& style = ImGui::GetStyle();

	style.WindowRounding	= 0.f;
	style.GrabRounding		= 0.f;
	style.TabRounding		= 0.f;
	style.ScrollbarRounding = 0.f;
	style.ChildRounding		= 0.f;
	style.FramePadding		= {5, 5};

	// http://paletton.com/#uid=12P0x0k0d9D00++cwOthgucrFnh
	const auto shade0 = ImColor(0.239f, 0.239f, 0.239f);
	const auto shade1 = ImColor(1.f, 1.f, 1.f);
	const auto shade2 = ImColor(0.561f, 0.922f, 0.561f);
	const auto shade3 = ImColor(0.349f, 0.757f, 0.349f);
	const auto shade4 = ImColor(0.078f, 0.58f, 0.078f);

	style.Colors[ImGuiCol_::ImGuiCol_TitleBgActive]	   = shade4;
	style.Colors[ImGuiCol_::ImGuiCol_ButtonActive]	   = shade4;
	style.Colors[ImGuiCol_::ImGuiCol_SliderGrab]	   = shade4;
	style.Colors[ImGuiCol_::ImGuiCol_ResizeGripActive] = shade4;
	style.Colors[ImGuiCol_::ImGuiCol_FrameBgActive]	   = shade4;
	style.Colors[ImGuiCol_::ImGuiCol_HeaderActive]	   = shade4;


	style.Colors[ImGuiCol_::ImGuiCol_ButtonHovered]		   = shade3;
	style.Colors[ImGuiCol_::ImGuiCol_FrameBgHovered]	   = shade3;
	style.Colors[ImGuiCol_::ImGuiCol_HeaderHovered]		   = shade3;
	style.Colors[ImGuiCol_::ImGuiCol_ResizeGripHovered]	   = shade3;
	style.Colors[ImGuiCol_::ImGuiCol_ScrollbarGrabHovered] = shade3;

	style.Colors[ImGuiCol_::ImGuiCol_ResizeGrip]  = shade2;
	style.Colors[ImGuiCol_::ImGuiCol_Button]	  = shade2;
	style.Colors[ImGuiCol_::ImGuiCol_SliderGrab]  = shade2;
	style.Colors[ImGuiCol_::ImGuiCol_ScrollbarBg] = shade2;
	style.Colors[ImGuiCol_::ImGuiCol_FrameBg]	  = shade2;
	style.Colors[ImGuiCol_::ImGuiCol_Header]	  = shade2;


	style.Colors[ImGuiCol_::ImGuiCol_CheckMark] = shade1;
}

//=================================================================================
void C_ImGuiLayer::OnDetach()
{
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
	auto	 window = Core::C_Application::Get().GetWndMgr().GetWindow(m_Window);
	ImGuiIO& io		= ::ImGui::GetIO();
	io.DisplaySize	= ImVec2(static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));

	::ImGui::NewFrame();
}

//=================================================================================
void C_ImGuiLayer::FrameEnd(Core::I_Input& input)
{
	UpdateMouseCursor(input);

	::ImGui::Render();
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
	d.Dispatch<Core::C_AppEvent>(std::bind(&C_ImGuiLayer::OnAppEvent, this, std::placeholders::_1));
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
	ImGuiIO& io						= ::ImGui::GetIO();
	io.KeysDown[event.GetKeyCode()] = true;

	io.KeyCtrl	= io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
	return io.WantCaptureKeyboard;
}

//=================================================================================
bool C_ImGuiLayer::OnKeyReleased(Core::C_KeyReleasedEvent& event)
{
	ImGuiIO& io						= ::ImGui::GetIO();
	io.KeysDown[event.GetKeyCode()] = false;

	io.KeyCtrl	= io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt	= io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
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
	ImGuiIO& io						 = ::ImGui::GetIO();
	io.MouseDown[e.GetMouseButton()] = true;

	return CapturingMouse();
}

//=================================================================================
bool C_ImGuiLayer::OnMouseButtonReleasedEvent(Core::C_MouseButtonReleased& e)
{
	ImGuiIO& io						 = ::ImGui::GetIO();
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

//=================================================================================
ImDrawData* C_ImGuiLayer::GetRenderData()
{
	return ::ImGui::GetDrawData();
}

//=================================================================================
bool C_ImGuiLayer::ReadyForDestroy() const
{
	return m_GUIMgr.CanBeDestroyed();
}

//=================================================================================
bool C_ImGuiLayer::OnAppEvent(Core::C_AppEvent& event)
{
	if (event.GetEventType() == Core::C_AppEvent::E_Type::WindowCloseRequest)
	{
		m_GUIMgr.RequestDestroy();
	}

	return false;
}

//=================================================================================
void C_ImGuiLayer::UpdateMouseCursor(Core::I_Input& input)
{
	ImGuiIO& io = ImGui::GetIO();
	if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0)
	{
		ImGuiMouseCursor_ imgui_cursor = static_cast<ImGuiMouseCursor_>(ImGui::GetMouseCursor());
		if (imgui_cursor != ImGuiMouseCursor_None)
		{
			switch (imgui_cursor)
			{
			case ImGuiMouseCursor_Arrow:
				input.SetMouseCursor(Core::I_Input::E_MouseCursor::Arrow);
				break;
			case ImGuiMouseCursor_Hand:
				input.SetMouseCursor(Core::I_Input::E_MouseCursor::Hand);
				break;
			case ImGuiMouseCursor_TextInput:
				input.SetMouseCursor(Core::I_Input::E_MouseCursor::TextInput);
				break;
			case ImGuiMouseCursor_ResizeNS:
				input.SetMouseCursor(Core::I_Input::E_MouseCursor::NSResize);
				break;
			case ImGuiMouseCursor_ResizeEW:
				input.SetMouseCursor(Core::I_Input::E_MouseCursor::WEResize);
				break;
			case ImGuiMouseCursor_ResizeAll:
			case ImGuiMouseCursor_ResizeNESW:
			case ImGuiMouseCursor_ResizeNWSE:
			case ImGuiMouseCursor_NotAllowed:
			default:
				input.SetMouseCursor(Core::I_Input::E_MouseCursor::Arrow);
				break;
			}
		}
	}
}

} // namespace GLEngine::GUI
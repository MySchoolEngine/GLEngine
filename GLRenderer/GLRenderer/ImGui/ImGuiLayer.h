#pragma once

#include <Core/EventSystem/Layer.h>

namespace GLEngine {

namespace Core {
class C_KeyPressedEvent;
class C_MouseScrollEvent;
class C_MouseButtonPressed;
class C_MouseButtonReleased;
class C_MouseMoved;
class C_KeyRepeatedEvent;
class C_KeyEvent;
}

namespace GLRenderer {
namespace ImGui {
class C_ImGuiLayer : public Core::C_Layer {
public:
	C_ImGuiLayer(GUID window);
	virtual ~C_ImGuiLayer();

	//=================================================================================
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate() override;

	void FrameBegin();

	void FrameEnd();

	virtual void OnEvent(Core::I_Event& event) override;
private:
	bool OnKeyPressed(Core::C_KeyPressedEvent& event);
	bool OnMouseButtonPressedEvent(Core::C_MouseButtonPressed& e);
	bool OnMouseButtonReleasedEvent(Core::C_MouseButtonReleased& e);
	bool OnMouseScrolledEvent(Core::C_MouseScrollEvent& e);
	bool OnMouseMoved(Core::C_MouseMoved& e);


	GUID	m_Window;
	float	m_Time;
};
}}}
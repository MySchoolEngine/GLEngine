#pragma once

#include <GUI/GUIManager.h>

#include <Core/EventSystem/Layer.h>

struct ImDrawData;

namespace GLEngine {

namespace Core {
class C_KeyPressedEvent;
class C_KeyReleasedEvent;
class C_KeyRepeatedEvent;
class C_KeyEvent;
class C_TextInputEvent;
class C_MouseScrollEvent;
class C_MouseButtonPressed;
class C_MouseButtonReleased;
class C_MouseMoved;
class C_AppEvent;
class I_Input;
} // namespace Core

namespace GUI {
class GUI_API_EXPORT C_ImGuiLayer : public Core::C_Layer {
public:
	C_ImGuiLayer(GUID window);
	~C_ImGuiLayer() override;

	//=================================================================================
	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;

	virtual void FrameBegin();

	virtual void FrameEnd(Core::I_Input& input);

	void OnEvent(Core::I_Event& event) override;

	[[nodiscard]] bool CapturingMouse() const;

	[[nodiscard]] C_GUIManager& GetGUIMgr();
	[[nodiscard]] bool			ReadyForDestroy() const override;

private:
	bool			   OnKeyPressed(Core::C_KeyPressedEvent& event);
	bool			   OnKeyReleased(Core::C_KeyReleasedEvent& event);
	bool			   OnTextEvent(Core::C_TextInputEvent& event);
	bool			   OnMouseButtonPressedEvent(Core::C_MouseButtonPressed& e);
	bool			   OnMouseButtonReleasedEvent(Core::C_MouseButtonReleased& e);
	bool			   OnMouseScrolledEvent(Core::C_MouseScrollEvent& e);
	bool			   OnMouseMoved(Core::C_MouseMoved& e);
	[[nodiscard]] bool OnAppEvent(Core::C_AppEvent& event);

protected:
	void UpdateMouseCursor(Core::I_Input& input);

	ImDrawData* GetRenderData();

	GUID		 m_Window;
	C_GUIManager m_GUIMgr;
	float		 m_Time;
};
} // namespace GUI
} // namespace GLEngine
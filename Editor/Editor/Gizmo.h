#pragma once

#include <Core/EventSystem/EventReciever.h>

namespace GLEngine::Core {
class I_Input;
class C_MouseButtonPressed;
class C_MouseButtonReleased;
class C_MouseMoved;
class C_KeyPressedEvent;
} // namespace GLEngine::Core

namespace GLEngine::Renderer {
class I_DebugDraw;
class I_CameraComponent;
} // namespace GLEngine::Renderer

namespace GLEngine::Editor {
class C_MousePickingHelper;

class C_Gizmo : Core::I_EventReceiver {
public:
	C_Gizmo(const glm::vec3& position, const Core::I_Input& input);

	[[nodiscard]] glm::vec3 GetPosition() const;
	void					SetPosition(const glm::vec3& position);

	void OnUpdate(const Renderer::I_CameraComponent& camera, C_MousePickingHelper& mouseHelper);
	void Draw(Renderer::I_DebugDraw& dd) const;

	[[nodiscard]] glm::vec3 GetPositionDiff() const;
	[[nodiscard]] bool		IsMouseOverGizmo() const;
	[[nodiscard]] bool		IsBeingControlled() const;

	void OnEvent(Core::I_Event& event) override;

private:
	[[nodiscard]] bool OnMouseKeyPressed(Core::C_MouseButtonPressed& event);
	[[nodiscard]] bool OnMouseKeyReleased(Core::C_MouseButtonReleased& event);
	[[nodiscard]] bool OnMouseMoved(Core::C_MouseMoved& event);
	[[nodiscard]] bool OnKeyPressed(Core::C_KeyPressedEvent& event);

	enum class E_Direction : std::uint8_t
	{
		Right,
		Up,
		Forward
	};
	[[nodiscard]] glm::vec3 GetAxe(const E_Direction dir) const;

	glm::vec3				   m_Position;
	glm::vec3				   m_DiffSinceLastFrame;
	std::optional<E_Direction> m_MouseOverDirection;
	bool					   m_ControllingGizmo;
	const Core::I_Input&	   m_Input;
	glm::vec2				   m_LastMousePosition; //< In clip space
	glm::vec2				   m_MouseOffset;		//< In clip space
};
} // namespace GLEngine::Editor

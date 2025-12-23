#include <EditorStdafx.h>

#include <Editor/Gizmo.h>
#include <Editor/Utils/MousePicking.h>

#include <Renderer/Colours.h>
#include <Renderer/DebugDraw.h>
#include <Renderer/ICameraComponent.h>

#include <Physics/Primitives/Plane.h>

#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/Event/MouseEvents.h>
#include <Core/EventSystem/EventDispatcher.h>
#include <Core/Input.h>

namespace GLEngine::Editor {

//=================================================================================
C_Gizmo::C_Gizmo(const glm::vec3& position, const Core::I_Input& input)
	: m_Position(position)
	, m_DiffSinceLastFrame(0.f)
	, m_ControllingGizmo(false)
	, m_Input(input)
	, m_LastMousePosition(0.f)
	, m_MouseOffset(0.f)
{
}

//=================================================================================
glm::vec3 C_Gizmo::GetPosition() const
{
	return m_Position;
}

//=================================================================================
void C_Gizmo::SetPosition(const glm::vec3& position)
{
	m_Position = position;
}

//=================================================================================
void C_Gizmo::OnUpdate(const Renderer::I_CameraComponent& camera, C_MousePickingHelper& mouseHelper)
{
	// do not change mouse over when dragging gizmo
	if (m_ControllingGizmo)
	{
		GLE_ASSERT(m_MouseOverDirection.has_value(), "When gizmo being controlled it needs to know which axe is being controlled");
		// 1) Reproject to the world
		// 2) Find the offset
		// 3) Move the position
		const auto ProjectedPosition = [&](const glm::vec2& mousePos, const glm::vec3& planeNormal) {
			const auto					 ray = camera.GetRay(mousePos);
			Physics::Primitives::S_Plane plane(planeNormal, m_Position);
			const auto					 depth = plane.IntersectImpl(ray);
			return ray.origin + ray.direction * depth;
		};

		const auto planeNormal = camera.GetPosition() - m_Position;

		const auto originalPos = ProjectedPosition(m_LastMousePosition, planeNormal);
		const auto newPos	   = ProjectedPosition(m_LastMousePosition + m_MouseOffset, planeNormal);
		m_DiffSinceLastFrame   = newPos - originalPos;

		m_DiffSinceLastFrame *= GetAxe(m_MouseOverDirection.value());

		m_Position += m_DiffSinceLastFrame;

		m_LastMousePosition = m_Input.GetClipSpaceMouseCoord();
		m_MouseOffset		= {0.f, 0.f};
		return;
	}

	mouseHelper.LineSegment(m_Position, m_Position + GetAxe(E_Direction::Right), [&]() { m_MouseOverDirection = E_Direction::Right; });
	mouseHelper.LineSegment(m_Position, m_Position + GetAxe(E_Direction::Up), [&]() { m_MouseOverDirection = E_Direction::Up; });
	mouseHelper.LineSegment(m_Position, m_Position + GetAxe(E_Direction::Forward), [&]() { m_MouseOverDirection = E_Direction::Forward; });

	m_MouseOverDirection.reset();
}

//=================================================================================
void C_Gizmo::Draw(Renderer::I_DebugDraw& dd) const
{
	const auto selectedMouseOverDirection = m_MouseOverDirection.has_value() ? static_cast<int>(m_MouseOverDirection.value()) : -1;
	dd.DrawLine(m_Position, m_Position + glm::vec3{1, 0, 0}, (selectedMouseOverDirection == 0) ? Colours::yellow : Colours::red);
	dd.DrawLine(m_Position, m_Position + glm::vec3{0, 1, 0}, (selectedMouseOverDirection == 1) ? Colours::yellow : Colours::green);
	dd.DrawLine(m_Position, m_Position + glm::vec3{0, 0, 1}, (selectedMouseOverDirection == 2) ? Colours::yellow : Colours::blue);
}

//=================================================================================
bool C_Gizmo::IsMouseOverGizmo() const
{
	return m_MouseOverDirection.has_value();
}

//=================================================================================
void C_Gizmo::OnEvent(Core::I_Event& event)
{
	Core::C_EventDispatcher d(event);
	d.Dispatch<Core::C_MouseButtonPressed>(std::bind(&C_Gizmo::OnMouseKeyPressed, this, std::placeholders::_1));
	d.Dispatch<Core::C_MouseButtonReleased>(std::bind(&C_Gizmo::OnMouseKeyReleased, this, std::placeholders::_1));
	d.Dispatch<Core::C_MouseMoved>(std::bind(&C_Gizmo::OnMouseMoved, this, std::placeholders::_1));
	d.Dispatch<Core::C_KeyPressedEvent>(std::bind(&C_Gizmo::OnKeyPressed, this, std::placeholders::_1));
}

//=================================================================================
bool C_Gizmo::OnMouseKeyPressed(Core::C_MouseButtonPressed& event)
{
	if (IsMouseOverGizmo())
	{
		m_ControllingGizmo	= true;
		m_LastMousePosition = m_Input.GetClipSpaceMouseCoord();
		return true;
	}
	return false;
}

//=================================================================================
bool C_Gizmo::OnMouseKeyReleased(Core::C_MouseButtonReleased& event)
{
	if (m_ControllingGizmo)
	{
		m_ControllingGizmo = false;
		return true;
	}
	return false;
}

//=================================================================================
bool C_Gizmo::OnKeyPressed(Core::C_KeyPressedEvent& event)
{
	return false;
}

//=================================================================================
bool C_Gizmo::OnMouseMoved(Core::C_MouseMoved& event)
{
	if (m_ControllingGizmo == false)
		return false;

	m_MouseOffset += m_Input.GetClipSpaceMouseCoord() - m_LastMousePosition;

	return true;
}

//=================================================================================
bool C_Gizmo::IsBeingControlled() const
{
	return m_ControllingGizmo;
}

//=================================================================================
glm::vec3 C_Gizmo::GetPositionDiff() const
{
	if (IsBeingControlled())
		return m_DiffSinceLastFrame;
	return {0.f, 0.f, 0.f};
}

//=================================================================================
glm::vec3 C_Gizmo::GetAxe(const E_Direction dir) const
{
	switch (dir)
	{
	case E_Direction::Right:
		return {1, 0, 0};
	case E_Direction::Up:
		return {0, 1, 0};
	case E_Direction::Forward:
		return {0, 0, 1};
	}
	GLE_ASSERT(false, "This should not happen");
	return {};
}

} // namespace GLEngine::Editor

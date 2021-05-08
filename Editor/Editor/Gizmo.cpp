#include <EditorStdAfx.h>

#include <Editor/Gizmo.h>
#include <Editor/Utils/MousePicking.h>

#include <Renderer/Colours.h>
#include <Renderer/DebugDraw.h>
#include <Renderer/ICameraComponent.h>

#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/Event/MouseEvents.h>
#include <Core/EventSystem/EventDispatcher.h>
#include <Core/Input.h>

namespace GLEngine::Editor {

//=================================================================================
C_Gizmo::C_Gizmo(const glm::vec3& position)
	: m_Position(position)
	, m_ControllingGizmo(false)
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
void C_Gizmo::OnUpdate(const Core::I_Input& input, const Renderer::I_CameraComponent& camera, C_MousePickingHelper& mouseHelper)
{
	// do not change mouse over when dragging gizmo
	if (m_ControllingGizmo)
		return;

	const auto mosuePosition = input.GetClipSpaceMouseCoord();

	mouseHelper.LineSegment(m_Position, m_Position + glm::vec3{1, 0, 0}, [&]() { m_MouseOverDirection = static_cast<E_Direction>(0); });
	mouseHelper.LineSegment(m_Position, m_Position + glm::vec3{0, 1, 0}, [&]() { m_MouseOverDirection = static_cast<E_Direction>(1); });
	mouseHelper.LineSegment(m_Position, m_Position + glm::vec3{0, 0, 1}, [&]() { m_MouseOverDirection = static_cast<E_Direction>(2); });

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
		m_ControllingGizmo = true;
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

	//todo:
	// 1) Reproject to the world
	// 2) Find the offset
	// 3) Move the position
	return true;
}

//=================================================================================
bool C_Gizmo::IsBeingControlled() const
{
	return m_ControllingGizmo;
}

} // namespace GLEngine::Editor

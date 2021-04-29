#include <EditorStdAfx.h>

#include <Editor/Gizmo.h>
#include <Editor/Utils/MousePicking.h>

#include <Renderer/Colours.h>
#include <Renderer/DebugDraw.h>
#include <Renderer/ICameraComponent.h>

#include <Core/Input.h>

namespace GLEngine::Editor {

//=================================================================================
C_Gizmo::C_Gizmo(const glm::vec3& position)
	: m_Position(position)
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
void C_Gizmo::OnUpdate(const Core::I_Input& input, const Renderer::I_CameraComponent& camera)
{
	std::array<float, 3> closestLines;

	const auto mosuePosition = input.GetClipSpaceMouseCoord();

	closestLines[0] = ScreenSpaceDistanceToLine(m_Position, m_Position + glm::vec3{1, 0, 0}, mosuePosition, camera);
	closestLines[1] = ScreenSpaceDistanceToLine(m_Position, m_Position + glm::vec3{0, 1, 0}, mosuePosition, camera);
	closestLines[2] = ScreenSpaceDistanceToLine(m_Position, m_Position + glm::vec3{0, 0, 1}, mosuePosition, camera);

	const auto it = std::min_element(closestLines.begin(), closestLines.end());
	m_MouseOverDirection.reset();
	if (it != closestLines.end() && *it < 0.1f)
		m_MouseOverDirection = static_cast<E_Direction>(std::distance(closestLines.begin(), it));
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

} // namespace GLEngine::Editor

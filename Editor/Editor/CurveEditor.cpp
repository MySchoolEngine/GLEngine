#include <EditorStdAfx.h>

#include <Editor/CurveEditor.h>
#include <Editor/Utils/MousePicking.h>

#include <Renderer/Colours.h>
#include <Renderer/DebugDraw.h>
#include <Renderer/ICameraComponent.h>
#include <Renderer/Mesh/Curve.h>

#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/Event/MouseEvents.h>
#include <Core/EventSystem/EventDispatcher.h>
#include <Core/Input.h>

#include <GLFW/glfw3.h>

namespace GLEngine::Editor {

//=================================================================================
C_CurveEditor::C_CurveEditor(Renderer::C_Curve& curve)
	: m_Curve(curve)
	, m_MouseOverPoint(-1)
	, m_SelectedPoint(-1)
	, m_MouseOverLineSegment(-1)
{
}

//=================================================================================
void C_CurveEditor::OnEvent(Core::I_Event& event)
{
	if (m_Gizmo)
		m_Gizmo->OnEvent(event);
	Core::C_EventDispatcher d(event);
	d.Dispatch<Core::C_MouseButtonPressed>(std::bind(&C_CurveEditor::OnMouseKeyPressed, this, std::placeholders::_1));
	d.Dispatch<Core::C_KeyPressedEvent>(std::bind(&C_CurveEditor::OnKeyPressed, this, std::placeholders::_1));
}

//=================================================================================
void C_CurveEditor::Draw(Renderer::I_DebugDraw& dd) const
{
	if (m_Curve.GetNumControlPoints() == 0)
		return;
	int	 i		  = 0;
	auto previous = m_Curve.GetControlPoint(0);
	m_Curve.ForEachControlPoint([&](const glm::vec3& current) {
		dd.DrawLine(previous, current, (m_MouseOverLineSegment == i) ? Colours::red : Colours::white);


		if (i == m_SelectedPoint)
		{
			dd.DrawPoint(current, Colours::blue);
		}
		else if (i == m_MouseOverPoint)
		{
			dd.DrawPoint(current, Colours::red);
		}
		else
		{
			dd.DrawPoint(current, Colours::white);
		}
		previous = current;

		++i;
	});
	if (m_Gizmo)
		m_Gizmo->Draw(dd);
}

//=================================================================================
void C_CurveEditor::OnUpdate(const Core::I_Input& input, const Renderer::I_CameraComponent& camera)
{
	if (m_Gizmo)
	{
		m_Gizmo->OnUpdate(input, camera);

		// if mouse is over gizmo I don't want to select anything from curve
		if (m_Gizmo->IsMouseOverGizmo())
		{
			m_MouseOverLineSegment = m_MouseOverPoint = -1;
			return;
		}
	}

	C_MousePickingHelper mousePicking(input, camera);

	std::vector<std::pair<int, float>> closestLineSegments;

	const auto mousePosition = input.GetClipSpaceMouseCoord();

	const auto numControlPoints = m_Curve.GetNumControlPoints();
	for (int i = 0; i < numControlPoints; ++i)
	{
		mousePicking.Point(m_Curve.GetControlPoint(i), [&, i]() { m_MouseOverPoint = i; });
		if (i > 0)
		{
			mousePicking.LineSegment(m_Curve.GetControlPoint(i - 1), m_Curve.GetControlPoint(i), [&, i]() { m_MouseOverLineSegment = i; });
		}
	}

	m_MouseOverPoint	   = -1;
	m_MouseOverLineSegment = -1;

	mousePicking.SelectInteraction();
}

//=================================================================================
bool C_CurveEditor::OnMouseKeyPressed(Core::C_MouseButtonPressed& event)
{
	if (event.GetMouseButton() == 0 && m_MouseOverPoint >= 0) // todo correct enum for mouse buttons
	{
		if (m_SelectedPoint == m_MouseOverPoint)
		{
			m_SelectedPoint = -1;
		}
		else
		{
			if (m_SelectedPoint != m_MouseOverPoint)
			{
				m_SelectedPoint = m_MouseOverPoint;
				m_Gizmo			= C_Gizmo(m_Curve.GetControlPoint(m_SelectedPoint));
			}
		}
		return true;
	}
	return false;
}

//=================================================================================
bool C_CurveEditor::OnKeyPressed(Core::C_KeyPressedEvent& event)
{
	if (event.GetKeyCode() == GLFW_KEY_DELETE && m_SelectedPoint >= 0)
	{
		m_Curve.RemoveControlPoint(m_SelectedPoint);
		m_SelectedPoint = -1;
		return true;
	}
	return false;
}

} // namespace GLEngine::Editor

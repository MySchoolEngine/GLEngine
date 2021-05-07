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

#include <Utils/StdVectorUtils.h>

#include <GLFW/glfw3.h>

namespace GLEngine::Editor {

//=================================================================================
C_CurveEditor::C_CurveEditor(Renderer::C_Curve& curve, const Core::I_Input& input)
	: m_Curve(curve)
	, m_Input(input)
	, m_MouseOverPoint(-1)
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
		if (m_MouseOverLineSegment == i)
		{
			dd.DrawLine(previous, current, Colours::red);
		}
		else if (IsLineSegmentSelected(i))
		{
			dd.DrawLine(previous, current, Colours::blue);
		}
		else
		{
			dd.DrawLine(previous, current, Colours::white);
		}

		if (i == m_MouseOverPoint)
		{
			dd.DrawPoint(current, Colours::red);
		}
		else if (IsPointSelected(i))
		{
			dd.DrawPoint(current, Colours::blue);
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
void C_CurveEditor::OnUpdate(const Renderer::I_CameraComponent& camera)
{
	C_MousePickingHelper mousePicking(m_Input, camera);

	if (m_Gizmo)
	{
		if (m_Gizmo->IsBeingControlled())
		{
			return;
		}
		m_Gizmo->OnUpdate(m_Input, camera, mousePicking);
	}

	const auto mousePosition = m_Input.GetClipSpaceMouseCoord();

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
	if (event.GetMouseButton() == GLFW_MOUSE_BUTTON_1) // todo correct enum for mouse buttons
	{
		// will be adding/removing from the set
		if (event.GetModifiers() & Core::E_KeyModifiers::Control)
		{
			if (m_MouseOverPoint >= 0)
			{
				if (IsPointSelected(m_MouseOverPoint))
				{
					RemovePointToSelected(m_MouseOverPoint);
				}
				else
				{
					AddPointToSelected(m_MouseOverPoint);
				}
			}
			else if (m_MouseOverLineSegment >= 0)
			{
				// if either of points is part of selection than add both
				// if one of the points is part of selection than add other
				// if both are part of the selection than remove both
				// if (IsPointSelected())
				if (IsLineSegmentSelected(m_MouseOverLineSegment)) {
					RemovePointToSelected(m_MouseOverLineSegment - 1);
					RemovePointToSelected(m_MouseOverLineSegment);
				}
				else
				{
					AddPointToSelected(m_MouseOverLineSegment - 1);
					AddPointToSelected(m_MouseOverLineSegment);
				}
			}
		}
		else
		{
			m_Selectedpoints.clear();
			if (m_MouseOverPoint >= 0)
			{
				AddPointToSelected(m_MouseOverPoint);
			}
			else if (m_MouseOverLineSegment >= 0)
			{
				AddPointToSelected(m_MouseOverLineSegment - 1);
				AddPointToSelected(m_MouseOverLineSegment);
			}
		}


		// update gizmo update
		return true;
	}
	return false;
}

//=================================================================================
bool C_CurveEditor::OnKeyPressed(Core::C_KeyPressedEvent& event)
{
	if (event.GetKeyCode() == GLFW_KEY_DELETE && !m_Selectedpoints.empty())
	{
		m_Curve.RemoveControlPoint(m_Selectedpoints);
		m_Selectedpoints.clear();
		return true;
	}
	else if (event.GetKeyCode() == GLFW_KEY_A && event.GetModifiers() & Core::E_KeyModifiers::Control)
	{
		m_Selectedpoints.clear();
		const auto curveLength = m_Curve.GetNumControlPoints();
		for (int i = 0; i < curveLength; ++i)
		{
			m_Selectedpoints.insert(i);
		}
		return true;
	}
	return false;
}

//=================================================================================
bool C_CurveEditor::IsPointSelected(std::size_t idx) const
{
	return Utils::contains(m_Selectedpoints, idx);
}

//=================================================================================
void C_CurveEditor::AddPointToSelected(std::size_t idx)
{
	m_Selectedpoints.insert(idx);
}

//=================================================================================
void C_CurveEditor::RemovePointToSelected(std::size_t idx)
{
	m_Selectedpoints.erase(idx);
}

//=================================================================================
bool C_CurveEditor::IsLineSegmentSelected(std::size_t idx) const
{
	return IsPointSelected(idx - 1) && IsPointSelected(idx);
}

} // namespace GLEngine::Editor

#include <EditorStdafx.h>

#include <Editor/Editors/WorldView/AABBEditor.h>
#include <Editor/Utils/MousePicking.h>

#include <Renderer/Colours.h>
#include <Renderer/DebugDraw.h>
#include <Renderer/ICameraComponent.h>
#include <Renderer/Render/CurveRenderer.h>
#include <Renderer/Viewport.h>

#include <GUI/ReflectionGUI.h>

#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/Event/MouseEvents.h>
#include <Core/EventSystem/EventDispatcher.h>
#include <Core/Input.h>

#include <Utils/StdVectorUtils.h>

#include <GLFW/glfw3.h>

namespace GLEngine::Editor {

//=================================================================================
C_AABBEditor::C_AABBEditor(Physics::Primitives::S_AABB& curve, const Core::I_Input& input)
	: I_EventReceiver()
	, m_AABB(curve)
	, m_Input(input)
	, m_MouseOverPoint(-1)
	, m_MouseOverLineSegment(-1)
{
}

//=================================================================================
void C_AABBEditor::OnEvent(Core::I_Event& event)
{
	if (m_Gizmo)
		m_Gizmo->OnEvent(event);
	Core::C_EventDispatcher d(event);
	d.Dispatch<Core::C_MouseButtonPressed>(std::bind(&C_AABBEditor::OnMouseKeyPressed, this, std::placeholders::_1));
}

//=================================================================================
void C_AABBEditor::Draw(Renderer::I_DebugDraw& dd) const
{
	int	 i		  = 0;
	auto previous = m_Curve.GetControlPoint(0);
	// Render the edited line for user to see what is being edited
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

	dd.DrawAABB(m_AABB, Colours::white);

	Renderer::C_3DCurveRenderer curveRenderer(dd);

	curveRenderer.Draw(*m_interpol, Colours::black, Colours::white, 100);

	if (m_Gizmo)
		m_Gizmo->Draw(dd);
}

//=================================================================================
void C_AABBEditor::OnUpdate(const Renderer::I_CameraComponent& camera, const Renderer::C_Viewport& viewport)
{
	C_MousePickingHelper mousePicking(m_Input, camera, viewport);

	if (m_Gizmo)
	{
		m_Gizmo->OnUpdate(camera, mousePicking);

		std::for_each(m_SelectedPoints.begin(), m_SelectedPoints.end(),
					  [&](const std::size_t idx) { m_Curve.SetControlPoint(idx, m_Curve.GetControlPoint(idx) + m_Gizmo->GetPositionDiff()); });

		if (m_Gizmo->IsBeingControlled())
			return;
	}

	// Feed all the interactable points to the helper
	const auto numControlPoints = m_Curve.GetNumControlPoints();
	for (unsigned int i = 0; i < numControlPoints; ++i)
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
bool C_AABBEditor::OnMouseKeyPressed(Core::C_MouseButtonPressed& event)
{
	if (event.GetMouseButton() == GLFW_MOUSE_BUTTON_1) // todo correct enum for mouse buttons
	{
		if (m_Gizmo && m_Gizmo->IsBeingControlled())
			return false;
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
				if (IsLineSegmentSelected(m_MouseOverLineSegment))
				{
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
			m_SelectedPoints.clear();
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

		UpdateGizmoPosition();
		return true;
	}
	return false;
}

//=================================================================================
bool C_AABBEditor::IsPointSelected(std::size_t idx) const
{
	return Utils::contains(m_SelectedPoints, idx);
}

//=================================================================================
void C_AABBEditor::AddPointToSelected(std::size_t idx)
{
	m_SelectedPoints.insert(idx);
}

//=================================================================================
void C_AABBEditor::RemovePointToSelected(std::size_t idx)
{
	m_SelectedPoints.erase(idx);
}

//=================================================================================
bool C_AABBEditor::IsLineSegmentSelected(std::size_t idx) const
{
	return IsPointSelected(idx - 1) && IsPointSelected(idx);
}

//=================================================================================
void C_AABBEditor::UpdateGizmoPosition()
{
	// no gizmo needed
	if (m_SelectedPoints.empty())
	{
		m_Gizmo.reset();
	}
	else
	{
		// 1] calculate mean point
		// 2] update position
		glm::vec3 meanPoint(0, 0, 0);
		for (const auto idx : m_SelectedPoints)
		{
			meanPoint += m_Curve.GetControlPoint(idx);
		}
		meanPoint /= m_SelectedPoints.size();
		if (!m_Gizmo)
		{
			m_Gizmo.emplace(meanPoint, m_Input);
		}
		else
		{
			m_Gizmo->SetPosition(meanPoint);
		}
	}
}

} // namespace GLEngine::Editor

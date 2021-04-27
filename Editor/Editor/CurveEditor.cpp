#include <EditorStdAfx.h>

#include <Editor/CurveEditor.h>

#include <Renderer/Colours.h>
#include <Renderer/DebugDraw.h>
#include <Renderer/ICameraComponent.h>
#include <Renderer/Mesh/Curve.h>

#include <Physics/GeometryUtils/DistanceToRay.h>

#include <Core/EventSystem/Event/MouseEvents.h>
#include <Core/EventSystem/EventDispatcher.h>
#include <Core/Input.h>

namespace GLEngine::Editor {

//=================================================================================
C_CurveEditor::C_CurveEditor(Renderer::C_Curve& curve)
	: m_Curve(curve)
	, m_MouseOverPoint(-1)
	, m_SelectedPoint(-1)
{
}

//=================================================================================
void C_CurveEditor::OnEvent(Core::I_Event& event)
{
	Core::C_EventDispatcher d(event);
	d.Dispatch<Core::C_MouseButtonPressed>(std::bind(&C_CurveEditor::OnMouseKeyPressed, this, std::placeholders::_1));
}

//=================================================================================
void C_CurveEditor::Draw(Renderer::I_DebugDraw& dd) const
{
	int	 i		  = 0;
	auto previous = m_Curve.GetControlPoint(0);
	m_Curve.ForEachControlPoint([&](const glm::vec3& current) {
		dd.DrawLine(previous, current, Colours::white);
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
}

//=================================================================================
void C_CurveEditor::OnUpdate(const Core::I_Input& input, std::shared_ptr<Renderer::I_CameraComponent> camera)
{
	std::vector<std::pair<int, float>> closestPoints;

	using namespace Physics::Primitives;
	const auto ray = camera->GetRay(input.GetClipSpaceMouseCoord());

	const auto numControlPoints = m_Curve.GetNumControlPoints();
	for (int i = 0; i < numControlPoints; ++i)
	{
		const auto distnace = Physics::DistanceToRay(m_Curve.GetControlPoint(i), ray);
		if (distnace < .1f)
		{
			closestPoints.emplace_back(i, distnace);
		}
	}

	m_MouseOverPoint = -1;

	std::sort(closestPoints.begin(), closestPoints.end(), [](const auto a, const auto b) { return a.second < b.second; });
	if (!closestPoints.empty())
		m_MouseOverPoint = closestPoints.begin()->first;
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
			m_SelectedPoint = m_MouseOverPoint;
		}
		return true;
	}
	return false;
}

} // namespace GLEngine::Editor

#include <EditorStdAfx.h>

#include <Editor/CurveEditor.h>

#include <Renderer/DebugDraw.h>
#include <Renderer/ICameraComponent.h>
#include <Renderer/Mesh/Curve.h>

#include <Physics/GeometryUtils/DistanceToRay.h>

#include <Core/Input.h>

namespace GLEngine::Editor {

//=================================================================================
C_CurveEditor::C_CurveEditor(Renderer::C_Curve& curve)
	: m_Curve(curve)
	, m_MouseOverPoint(-1)
{
}

//=================================================================================
void C_CurveEditor::OnEvent(Core::I_Event& event)
{
}

//=================================================================================
void C_CurveEditor::Draw(Renderer::I_DebugDraw& dd) const
{
	int	 i		  = 0;
	auto previous = m_Curve.GetControlPoint(0);
	m_Curve.ForEachControlPoint([&](const glm::vec3& current) {
		dd.DrawLine(previous, current, glm::vec3(1, 0, 0));
		dd.DrawPoint(current, (i == m_MouseOverPoint) ? glm::vec3(1, 0, 0) : glm::vec3(1, 1, 1));
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

} // namespace GLEngine::Editor

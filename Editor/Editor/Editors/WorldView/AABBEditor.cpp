#include <EditorStdafx.h>

#include <Editor/Editors/WorldView/AABBEditor.h>
#include <Editor/Utils/MousePicking.h>

#include <Renderer/Colours.h>
#include <Renderer/DebugDraw.h>
#include <Renderer/ICameraComponent.h>
#include <Renderer/Render/CurveRenderer.h>
#include <Renderer/Viewport.h>

#include <GUI/ReflectionGUI.h>

#include <Physics/Primitives/AABB.h>

#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/Event/MouseEvents.h>
#include <Core/EventSystem/EventDispatcher.h>
#include <Core/Input.h>

#include <Utils/StdVectorUtils.h>

#include <GLFW/glfw3.h>

namespace GLEngine::Editor {

//=================================================================================
C_AABBEditor::C_AABBEditor(Physics::Primitives::S_AABB& aabb, const Core::I_Input& input)
	: I_EventReceiver()
	, m_AABB(aabb)
	, m_Input(input)
	, m_MouseOverLineSegment({})
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
	ForEachEdge([&](const AABBEdges edge) {
		const auto [begin, end]	 = GetEdge(edge);
		Colours::T_Colour colour = Colours::white;
		if (m_MouseOverLineSegment.has_value() && m_MouseOverLineSegment.value() == edge)
		{
			colour = Colours::Editing::mouseOver;
		}
		if (m_SelectedEdge.has_value() && m_SelectedEdge.value() == edge)
		{
			colour = Colours::Editing::selected;
		}
		dd.DrawLine(begin, end, colour);
	});

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

		// std::for_each(m_SelectedPoints.begin(), m_SelectedPoints.end(),
		// 			  [&](const std::size_t idx) { m_Curve.SetControlPoint(idx, m_Curve.GetControlPoint(idx) + m_Gizmo->GetPositionDiff()); });

		if (m_Gizmo->IsBeingControlled())
			return;
	}

	ForEachEdge([&](const AABBEdges edge) {
		const auto [begin, end] = GetEdge(edge);
		mousePicking.LineSegment(begin, end, [&, edge]() { m_MouseOverLineSegment = edge; });
	});

	m_MouseOverLineSegment = {};

	mousePicking.SelectInteraction();
}

//=================================================================================
bool C_AABBEditor::OnMouseKeyPressed(Core::C_MouseButtonPressed& event)
{
	if (event.GetMouseButton() == GLFW_MOUSE_BUTTON_1) // todo correct enum for mouse buttons
	{
		if (m_Gizmo && m_Gizmo->IsBeingControlled())
			return false;

		if (m_MouseOverLineSegment.has_value())
		{
			m_SelectedEdge = m_MouseOverLineSegment.value();
		}


		UpdateGizmoPosition();
		return true;
	}
	return false;
}

//=================================================================================
void C_AABBEditor::UpdateGizmoPosition()
{
	// no gizmo needed
	if (m_SelectedEdge.has_value() == false)
	{
		m_Gizmo.reset();
	}
	else
	{
		if (m_SelectedEdge.has_value())
		{
			const auto [begin, end] = GetEdge(m_SelectedEdge.value());
			const auto center		= (begin + end) / 2.f;

			m_Gizmo.emplace(center, m_Input);
		}
	}
}

//=================================================================================
template <class Func> void C_AABBEditor::ForEachEdge(const Func& fnc)
{
	using type = std::underlying_type_t<AABBEdges>;
	for (type i = 0; i < std::to_underlying(AABBEdges::Last); ++i)
	{
		fnc(static_cast<AABBEdges>(i));
	}
}

//=================================================================================
std::array<glm::vec3, 2> C_AABBEditor::GetEdge(const AABBEdges edge) const
{
	const auto left	 = m_AABB.m_Min.x;
	const auto right = m_AABB.m_Max.x;

	const auto top	  = m_AABB.m_Max.y;
	const auto bottom = m_AABB.m_Min.y;

	const auto front = m_AABB.m_Min.z;
	const auto back	 = m_AABB.m_Max.z;

	switch (edge)
	{
	case AABBEdges::TopLeft:
		return {{{left, top, front}, {left, top, back}}};
	case AABBEdges::TopRight:
		return {{{right, top, front}, {right, top, back}}};
	case AABBEdges::TopFront:
		return {{{left, top, front}, {right, top, front}}};
	case AABBEdges::TopBack:
		return {{{left, top, back}, {right, top, back}}};
	case AABBEdges::BottomLeft:
		return {{{left, bottom, front}, {left, bottom, back}}};
	case AABBEdges::BottomRight:
		return {{{right, bottom, front}, {right, bottom, back}}};
	case AABBEdges::BottomFront:
		return {{{left, bottom, front}, {right, bottom, front}}};
	case AABBEdges::BottomBack:
		return {{{left, bottom, back}, {right, bottom, back}}};
	case AABBEdges::FrontLeft:
		return {{{left, top, front}, {left, bottom, front}}};
	case AABBEdges::FrontRight:
		return {{{right, top, front}, {right, bottom, front}}};
	case AABBEdges::BackLeft:
		return {{{left, top, back}, {left, bottom, back}}};
	case AABBEdges::BackRight:
		return {{{right, top, back}, {right, bottom, back}}};
	}

	GLE_ERROR("Unsupported value");
	return {};
}

} // namespace GLEngine::Editor

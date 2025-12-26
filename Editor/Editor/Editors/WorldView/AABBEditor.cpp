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

		// apply movement
		ApplyOffset(m_Gizmo->GetPositionDiff());

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
			m_SelectedEdge = m_MouseOverLineSegment;
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
			m_Gizmo->DisableDir(DisabledDirection(*m_SelectedEdge));
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

//=================================================================================
C_Gizmo::E_Direction C_AABBEditor::DisabledDirection(AABBEdges edge)
{
	switch (edge)
	{
		using enum C_Gizmo::E_Direction;

	case AABBEdges::TopLeft:
	case AABBEdges::TopRight:
		return Forward;

	case AABBEdges::TopFront:
	case AABBEdges::TopBack:
		return Right;

	case AABBEdges::FrontLeft:
	case AABBEdges::FrontRight:
	case AABBEdges::BackLeft:
	case AABBEdges::BackRight:
		return Up;

	case AABBEdges::BottomLeft:
	case AABBEdges::BottomRight:
		return Forward;

	case AABBEdges::BottomFront:
	case AABBEdges::BottomBack:
		return Right;
	}

	GLE_ERROR("Unsupported value");
	return {};
}

//=================================================================================
void C_AABBEditor::ApplyOffset(const glm::vec3& offset)
{
	GLE_ASSERT(m_SelectedEdge.has_value(), "trying to move edge when not selected");
	// break it by the axis!

	// x -> left to right
	if (offset.x != 0.f)
	{
		switch (*m_SelectedEdge)
		{
			using enum AABBEdges;
		case BottomLeft:
		case BackLeft:
		case FrontLeft:
		case TopLeft: {
			m_AABB.m_Min.x += offset.x;
			if (m_AABB.m_Min.x > m_AABB.m_Max.x)
			{
				m_SelectedEdge = SwapByDirection(C_Gizmo::E_Direction::Right, *m_SelectedEdge);
				std::swap(m_AABB.m_Min.x, m_AABB.m_Max.x);
			}
			break;
		}

		case TopRight:
		case FrontRight:
		case BackRight:
		case BottomRight: {
			m_AABB.m_Max.x += offset.x;
			if (m_AABB.m_Min.x > m_AABB.m_Max.x)
			{
				m_SelectedEdge = SwapByDirection(C_Gizmo::E_Direction::Right, *m_SelectedEdge);
				std::swap(m_AABB.m_Min.x, m_AABB.m_Max.x);
			}
			break;
		}
		default:
			break;
		}
		return;
	}

	// y -> Bottom -> top
	if (offset.y != 0.f)
	{
		switch (*m_SelectedEdge)
		{
			using enum AABBEdges;
		case BottomLeft:
		case BottomRight:
		case BottomFront:
		case BottomBack: {
			m_AABB.m_Min.y += offset.y;
			if (m_AABB.m_Min.y > m_AABB.m_Max.y)
			{
				m_SelectedEdge = SwapByDirection(C_Gizmo::E_Direction::Up, *m_SelectedEdge);
				std::swap(m_AABB.m_Min.y, m_AABB.m_Max.y);
			}
			break;
		}
		case TopLeft:
		case TopRight:
		case TopFront:
		case TopBack: {
			m_AABB.m_Max.y += offset.y;
			if (m_AABB.m_Min.y > m_AABB.m_Max.y)
			{
				m_SelectedEdge = SwapByDirection(C_Gizmo::E_Direction::Up, *m_SelectedEdge);
				std::swap(m_AABB.m_Min.y, m_AABB.m_Max.y);
			}
			break;
		}
		default:
			break;
		}
		return;
	}

	// z -> Front to back
	if (offset.z != 0.f)
	{
		switch (*m_SelectedEdge)
		{
			using enum AABBEdges;

		case TopFront:
		case BottomFront:
		case FrontLeft:
		case FrontRight: {
			m_AABB.m_Min.z += offset.z;
			if (m_AABB.m_Min.z > m_AABB.m_Max.z)
			{
				m_SelectedEdge = SwapByDirection(C_Gizmo::E_Direction::Forward, *m_SelectedEdge);
				std::swap(m_AABB.m_Min.z, m_AABB.m_Max.z);
			}
			break;
		}
		case TopBack:
		case BottomBack:
		case BackLeft:
		case BackRight: {
			m_AABB.m_Max.z += offset.z;
			if (m_AABB.m_Min.z > m_AABB.m_Max.z)
			{
				m_SelectedEdge = SwapByDirection(C_Gizmo::E_Direction::Forward, *m_SelectedEdge);
				std::swap(m_AABB.m_Min.z, m_AABB.m_Max.z);
			}
			break;
		}
		default:
			break;
		}
		return;
	}
}

//=================================================================================
C_AABBEditor::AABBEdges C_AABBEditor::SwapByDirection(C_Gizmo::E_Direction dir, AABBEdges edge)
{
	switch (dir)
	{
		using enum C_Gizmo::E_Direction;
		using enum AABBEdges;
	case Right:
		switch (edge)
		{
		case BottomLeft:
			return BottomRight;
		case BackLeft:
			return BackRight;
		case FrontLeft:
			return FrontRight;
		case TopLeft:
			return TopRight;
		case BottomRight:
			return BottomLeft;
		case BackRight:
			return BackLeft;
		case FrontRight:
			return FrontLeft;
		case TopRight:
			return TopLeft;
		default:
			GLE_ERROR("Cannot be switched by this direction");
		}
		break;
	case Up:
		switch (edge)
		{
		case TopLeft:
			return BottomLeft;
		case TopRight:
			return BottomRight;
		case TopFront:
			return BottomFront;
		case TopBack:
			return BottomBack;
		case BottomLeft:
			return TopLeft;
		case BottomRight:
			return TopRight;
		case BottomFront:
			return TopFront;
		case BottomBack:
			return TopBack;
		default:
			GLE_ERROR("Cannot be switched by this direction");
		}
		break;
	case Forward:
		switch (edge)
		{
		case TopFront:
			return TopBack;
		case TopBack:
			return TopFront;
		case FrontLeft:
			return BackLeft;
		case FrontRight:
			return BackRight;
		case BackLeft:
			return FrontLeft;
		case BackRight:
			return FrontRight;
		case BottomFront:
			return BottomBack;
		case BottomBack:
			return BottomFront;
		default:
			GLE_ERROR("Cannot be switched by this direction");
		}
		break;
	}
	return {};
}

} // namespace GLEngine::Editor

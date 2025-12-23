#pragma once

#include <Editor/EditorApi.h>
#include <Editor/Gizmo.h>

#include <Renderer/Mesh/Curve.h>

#include <Core/EventSystem/EventReciever.h>

#include <rttr/registration_friend.h>

namespace GLEngine::Physics::Primitives {
struct S_AABB;
}

namespace GLEngine::Core {
class I_Input;
class C_MouseButtonPressed;
class C_KeyPressedEvent;
} // namespace GLEngine::Core

namespace GLEngine::Renderer {
class I_DebugDraw;
class I_CameraComponent;
class C_Viewport;
} // namespace GLEngine::Renderer

namespace GLEngine::Editor {

class EDITOR_API_EXPORT C_AABBEditor : public Core::I_EventReceiver {
public:
	C_AABBEditor(Physics::Primitives::S_AABB& curve, const Core::I_Input& input);

	void OnEvent(Core::I_Event& event) override;
	void OnUpdate(const Renderer::I_CameraComponent& camera, const Renderer::C_Viewport& viewport);

	void Draw(Renderer::I_DebugDraw& dd) const;

private:
	bool OnMouseKeyPressed(Core::C_MouseButtonPressed& event);

	void			   AddPointToSelected(std::size_t idx);
	void			   RemovePointToSelected(std::size_t idx);
	[[nodiscard]] bool IsPointSelected(std::size_t idx) const;
	void			   UpdateGizmoPosition();
	/**
	 * Line segments are indexed from 1. Index n means that points n-1 and n is selected.
	 */
	[[nodiscard]] bool			 IsLineSegmentSelected(std::size_t idx) const;
	Physics::Primitives::S_AABB& m_AABB;
	std::optional<C_Gizmo>		 m_Gizmo;
	const Core::I_Input&		 m_Input;

	int					  m_MouseOverPoint;
	std::set<std::size_t> m_SelectedPoints;

	int m_MouseOverLineSegment;
};
} // namespace GLEngine::Editor
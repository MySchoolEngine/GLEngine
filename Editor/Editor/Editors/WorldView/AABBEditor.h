#pragma once

#include <Editor/EditorApi.h>
#include <Editor/Gizmo.h>

#include <Core/EventSystem/EventReciever.h>

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
	C_AABBEditor(Physics::Primitives::S_AABB& aabb, const Core::I_Input& input);

	void OnEvent(Core::I_Event& event) override;
	void OnUpdate(const Renderer::I_CameraComponent& camera, const Renderer::C_Viewport& viewport);

	void Draw(Renderer::I_DebugDraw& dd) const;

private:
	bool OnMouseKeyPressed(Core::C_MouseButtonPressed& event);

	void			   UpdateGizmoPosition();

	Physics::Primitives::S_AABB& m_AABB;
	std::optional<C_Gizmo>		 m_Gizmo;
	const Core::I_Input&		 m_Input;

	// front for those enums is -z
	enum class AABBEdges : std::uint8_t {
		TopLeft = 0,
		TopRight,
		TopFront,
		TopBack,

		// sides
		FrontLeft,
		FrontRight,
		BackLeft,
		BackRight,

		BottomLeft,
		BottomRight,
		BottomFront,
		BottomBack,

		Last,
	};

	std::array<glm::vec3, 2> GetEdge(const AABBEdges edge) const;
	template<class Func>
	static void ForEachEdge(const Func& fnc);

	static C_Gizmo::E_Direction DisabledDirection(AABBEdges edge);

	std::optional<AABBEdges> m_SelectedEdge;


	std::optional<AABBEdges> m_MouseOverLineSegment;
};
} // namespace GLEngine::Editor
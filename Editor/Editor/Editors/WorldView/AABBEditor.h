#pragma once

#include <Editor/EditorApi.h>
#include <Editor/Gizmo.h>

#include <Renderer/Mesh/Curve.h>

#include <Core/EventSystem/EventReciever.h>

#include <rttr/registration_friend.h>

namespace GLEngine::Core {
class I_Input;
class C_MouseButtonPressed;
class C_KeyPressedEvent;
} // namespace GLEngine::Core

namespace GLEngine::Renderer {
class C_Curve;
class I_DebugDraw;
class I_CameraComponent;
class C_Viewport;
} // namespace GLEngine::Renderer

namespace GLEngine::Editor {

class EDITOR_API_EXPORT C_CurveEditor : public Core::I_EventReceiver {
public:
	C_CurveEditor(Renderer::C_Curve& curve, const Core::I_Input& input);

	virtual void OnEvent(Core::I_Event& event) override;
	void		 OnUpdate(const Renderer::I_CameraComponent& camera, const Renderer::C_Viewport& viewport);

	void Draw(Renderer::I_DebugDraw& dd) const;

	RTTR_REGISTRATION_FRIEND
private:
	bool OnMouseKeyPressed(Core::C_MouseButtonPressed& event);
	bool OnKeyPressed(Core::C_KeyPressedEvent& event);

	void			   AddPointToSelected(std::size_t idx);
	void			   RemovePointToSelected(std::size_t idx);
	[[nodiscard]] bool IsPointSelected(std::size_t idx) const;
	void			   UpdateGizmoPosition();
	/**
	 * Line segments are indexed from 1. Index n means that points n-1 and n is selected.
	 */
	[[nodiscard]] bool	   IsLineSegmentSelected(std::size_t idx) const;
	Renderer::C_Curve&	   m_Curve;
	std::optional<C_Gizmo> m_Gizmo;
	const Core::I_Input&   m_Input;


	enum class E_InterpolationType
	{
		Linear,
		Bezier,
		SmoothBezier,
	};

	E_InterpolationType m_Select;

	int					  m_MouseOverPoint;
	std::set<std::size_t> m_SelectedPoints;

	int m_MouseOverLineSegment;

	mutable std::unique_ptr<Renderer::I_CurveFunction<glm::vec3>> m_interpol;
};
} // namespace GLEngine::Editor
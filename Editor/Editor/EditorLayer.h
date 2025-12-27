#pragma once

#include <Editor/EditorApi.h>
#include <Editor/Gizmo.h>

#include <Renderer/Viewport.h>

#include <Core/EventSystem/Layer.h>

namespace GLEngine::Entity {
class C_EntityManager;
}

namespace GLEngine::Physics::Primitives {
struct S_AABB;
}

namespace GLEngine::Renderer {
class I_Renderer;
class I_DebugDraw;
class C_Curve;
class I_CameraComponent;
} // namespace GLEngine::Renderer

namespace GLEngine::Core {
class I_Input;
} // namespace GLEngine::Core

// This class will be main point of editor communication
// it should decide which element will receive event
// hold gizmos manager, know about selected 3D entities
// decide which model will render its AABB box etc.
namespace GLEngine::Editor {
class C_Gizmo;
class C_AABBEditor;
class C_CurveEditor;
class C_AnimationEditor;

// TODO update camera on world change???

class EDITOR_API_EXPORT C_EditorLayer : public Core::C_Layer {
public:
	C_EditorLayer(Renderer::I_DebugDraw& dd, const Core::I_Input& directInput, const Renderer::C_Viewport& viewport, const std::shared_ptr<Entity::C_EntityManager>& world);
	~C_EditorLayer() override;
	void SetCamera(const std::shared_ptr<Renderer::I_CameraComponent>& camera);
	void OnUpdate() override;
	void SetViewport(const Renderer::C_Viewport& viewport);
	void SetWorld(std::shared_ptr<Entity::C_EntityManager> world);

	void EditCurve(Renderer::C_Curve& curve);
	void EditAABB(Physics::Primitives::S_AABB& aabb);

	void EditAnimation(); // TODO!

	void OnEvent(Core::I_Event& event) override;
	bool OnEntityEvent(Core::C_EntityEvent& event);

private:
	Renderer::I_DebugDraw&					   m_dd;
	const Core::I_Input&					   m_Input;
	std::weak_ptr<Renderer::I_CameraComponent> m_Camera;
	Renderer::C_Viewport					   m_Viewport;

	std::weak_ptr<Entity::C_EntityManager>	   m_World;
	GUID									   m_SelectedEntity;
	std::optional<C_Gizmo>					   m_EntityGizmo;

	std::unique_ptr<C_CurveEditor>	   m_CurveEditing;
	std::unique_ptr<C_AABBEditor>	   m_AABBEditing;
	std::unique_ptr<C_AnimationEditor> m_AnimationEditing;
};
} // namespace GLEngine::Editor

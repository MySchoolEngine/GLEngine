#pragma once

#include <Editor/EditorApi.h>

#include <Renderer/Viewport.h>

#include <Core/EventSystem/Layer.h>

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
class C_CurveEditor;

class EDITOR_API_EXPORT C_EditorLayer : public Core::C_Layer {
public:
	C_EditorLayer(Renderer::I_DebugDraw& dd, const Core::I_Input& directInput, const Renderer::C_Viewport& viewport);
	~C_EditorLayer();
	void		 SetCamera(std::shared_ptr<Renderer::I_CameraComponent> camera);
	virtual void OnEvent(Core::I_Event& event) override;
	virtual void OnUpdate() override;
	void		 SetViewport(const Renderer::C_Viewport& viewport);

	void EditCurve(Renderer::C_Curve& curve);

private:
	Renderer::I_DebugDraw&					   m_dd;
	const Core::I_Input&					   m_Input;
	std::weak_ptr<Renderer::I_CameraComponent> m_Camera;
	Renderer::C_Viewport					   m_Viewport;

	std::unique_ptr<C_CurveEditor> m_CurveEditing;
};
} // namespace GLEngine::Editor

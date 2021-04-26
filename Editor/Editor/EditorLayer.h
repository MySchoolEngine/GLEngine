#pragma once

#include <Editor/EditorApi.h>

#include <Core/EventSystem/Layer.h>

namespace GLEngine::Renderer {
class I_Renderer;
class I_DebugDraw;
} // namespace GLEngine::Renderer
namespace GLEngine::Core {
class I_Input;
} // namespace GLEngine::Core

// This class will be main point of editor communication
// it should decide which element will receive event
// hold gizmos manager, know about selected 3D entities
// decide which model will render its AABB box etc.
namespace GLEngine::Editor {
class EDITOR_API_EXPORT C_EditorLayer : public Core::C_Layer {
public:
	C_EditorLayer(Renderer::I_DebugDraw& dd, Core::I_Input& directInput);

	virtual void OnEvent(Core::I_Event& event) override;
	virtual void OnUpdate() override;

	void EditCurve(Renderer::C_Curve& curve);

private:
	Renderer::I_DebugDraw& m_dd;
	Core::I_Input&		   m_Input
};
} // namespace GLEngine::Editor

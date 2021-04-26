#pragma once

#include <Editor/EditorApi.h>

#include <Core/EventSystem/EventReciever.h>

namespace GLEngine::Renderer {
class I_Renderer;
class I_DebugDraw;
} // namespace GLEngine::Renderer

// This class will be main point of editor communication
// it should decide which element will receive event
// hold gizmos manager, know about selected 3D entities
// decide which model will render its AABB box etc.
namespace GLEngine::Editor {
class EDITOR_API_EXPORT C_EditorLayer : public Core::I_EventReciever {
public:
	C_EditorLayer(Renderer::I_DebugDraw& dd);

private:
	Renderer::I_DebugDraw& m_dd;
};
} // namespace GLEngine::Editor

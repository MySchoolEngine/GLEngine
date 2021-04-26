#include <EditorStdafx.h>

#include <Editor/EditorLayer.h>

#include <Renderer/DebugDraw.h>

#include <Core/EventSystem/EventDispatcher.h>

namespace GLEngine::Editor {

//=================================================================================
C_EditorLayer::C_EditorLayer(Renderer::I_DebugDraw& dd, Core::I_Input& directInput)
	: Core::C_Layer("Editor Layer")
	, m_dd(dd)
	, m_Input(directInput)
{
}

//=================================================================================
void C_EditorLayer::OnEvent(Core::I_Event& event)
{
}

//=================================================================================
void C_EditorLayer::OnUpdate()
{

}

}; // namespace GLEngine::Editor
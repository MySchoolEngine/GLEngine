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
	if (m_CurveEditing)
		m_CurveEditing->Draw(m_dd);
}

//=================================================================================
void C_EditorLayer::EditCurve(Renderer::C_Curve& curve)
{
	m_CurveEditing = std::make_unique<C_CurveEditor>(curve);
}

}; // namespace GLEngine::Editor
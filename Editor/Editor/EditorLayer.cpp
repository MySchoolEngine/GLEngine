#include <EditorStdafx.h>

#include <Editor/AnimationEditor.h>
#include <Editor/EditorLayer.h>
#include <Editor/Editors/WorldView/AABBEditor.h>
#include <Editor/Editors/WorldView/CurveEditor.h>

#include <Renderer/DebugDraw.h>

#include <Core/EventSystem/EventDispatcher.h>

namespace GLEngine::Editor {

//=================================================================================
C_EditorLayer::C_EditorLayer(Renderer::I_DebugDraw& dd, const Core::I_Input& directInput, const Renderer::C_Viewport& viewport)
	: Core::C_Layer("Editor Layer")
	, m_dd(dd)
	, m_Input(directInput)
	, m_Viewport(viewport)
	, m_CurveEditing(nullptr)
	, m_AnimationEditing(nullptr)
{
}

//=================================================================================
C_EditorLayer::~C_EditorLayer() = default;

//=================================================================================
void C_EditorLayer::OnEvent(Core::I_Event& event)
{
	if (m_CurveEditing)
		m_CurveEditing->OnEvent(event);
	if (m_AABBEditing)
		m_AABBEditing->OnEvent(event);
}

//=================================================================================
void C_EditorLayer::OnUpdate()
{
	if (m_CurveEditing)
	{
		m_CurveEditing->Draw(m_dd);
		if (auto camera = m_Camera.lock())
			m_CurveEditing->OnUpdate(*camera.get(), m_Viewport);
	}
	if (m_AABBEditing)
	{
		m_AABBEditing->Draw(m_dd);
		if (auto camera = m_Camera.lock())
			m_AABBEditing->OnUpdate(*camera.get(), m_Viewport);
	}


	if (m_AnimationEditing)
		m_AnimationEditing->DrawGUI();
}

//=================================================================================
void C_EditorLayer::EditCurve(Renderer::C_Curve& curve)
{
	m_CurveEditing = std::make_unique<C_CurveEditor>(curve, m_Input);
}

//=================================================================================
void C_EditorLayer::EditAABB(Physics::Primitives::S_AABB& aabb)
{
	m_AABBEditing = std::make_unique<C_AABBEditor>(aabb, m_Input);
}

//=================================================================================
void C_EditorLayer::SetCamera(const std::shared_ptr<Renderer::I_CameraComponent>& camera)
{
	m_Camera = camera;
}

//=================================================================================
void C_EditorLayer::SetViewport(const Renderer::C_Viewport& viewport)
{
	m_Viewport = viewport;
}

//=================================================================================
void C_EditorLayer::EditAnimation()
{
	m_AnimationEditing = std::make_unique<C_AnimationEditor>();
}

}; // namespace GLEngine::Editor
#include <EditorStdAfx.h>

#include <Editor/CurveEditor.h>

#include <Renderer/DebugDraw.h>
#include <Renderer/Mesh/Curve.h>

namespace GLEngine::Editor {

//=================================================================================
C_CurveEditor::C_CurveEditor(Renderer::C_Curve& curve)
	: m_Curve(curve)
{
}

//=================================================================================
void C_CurveEditor::OnEvent(Core::I_Event& event)
{
}

//=================================================================================
void C_CurveEditor::Draw(Renderer::I_DebugDraw& dd) const
{
	auto previous = m_Curve.GetControlPoint(0);
	m_Curve.ForEachControlPoint([&](const glm::vec3& current) { 
		dd.DrawLine(previous, current, glm::vec3(1, 0, 0));
		dd.DrawPoint(current, glm::vec3(1,1,1));
		previous = current;
	});
}

} // namespace GLEngine::Editor

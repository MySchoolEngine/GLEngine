#include <RendererStdafx.h>

#include <Renderer/Renderer2D.h>

namespace GLEngine::Renderer {

//=================================================================================
void C_Renderer2D::Draw(RenderCall2D&& call)
{
	m_DrawCalls.emplace_back(std::move(call));
}

//=================================================================================
void C_Renderer2D::Clear()
{
	m_DrawCalls.clear();
}

//=================================================================================
void C_Renderer2D::Commit(I_RenderInterface2D& interface)
{
	for (const RenderCall2D& call : m_DrawCalls)
	{
		// bind the shader,
		interface.Render(call);
	}
}
} // namespace GLEngine::Renderer
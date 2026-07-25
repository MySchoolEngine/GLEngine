#include <RendererStdafx.h>

#include <Renderer/ComputeRenderer.h>

namespace GLEngine::Renderer {

//=================================================================================
void C_ComputeRenderer::Draw(ComputeRenderCall&& call)
{
	m_DrawCalls.emplace_back(std::move(call));
}

//=================================================================================
void C_ComputeRenderer::Clear()
{
	m_DrawCalls.clear();
}

//=================================================================================
void C_ComputeRenderer::Commit(I_ComputeRenderInterface& interface)
{
	for (const auto& call : m_DrawCalls)
	{
		// bind the shader,
		interface.Render(call);
	}
	m_DrawCalls.clear();
}
} // namespace GLEngine::Renderer
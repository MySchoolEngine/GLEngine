#include <RendererStdafx.h>

#include <Renderer/Renderer3D.h>

namespace GLEngine::Renderer {

//=================================================================================
void Renderer3D::Draw(RenderCall3D&& call)
{
	m_DrawCalls.emplace_back(std::move(call));
}

//=================================================================================
void Renderer3D::Clear()
{
	m_DrawCalls.clear();
}

//=================================================================================
void Renderer3D::Commit(RenderInterface& interface)
{
	// please, KISS, I want to see improvements as I go. No sorting, no binning, no instancing.
	// I will implement it later
	for (const RenderCall3D& call : m_DrawCalls) {
		// bind the shader,
		interface.Render(call);
	}
	m_DrawCalls.clear();
}

} // namespace GLEngine::Renderer
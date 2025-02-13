#pragma once

#include <Renderer/Renderer2D.h>

namespace GLEngine::GLRenderer {
class C_GLRendererInterface2D final : public Renderer::I_RenderInterface2D {
public:
	explicit C_GLRendererInterface2D(const glm::uvec2 size);
	explicit C_GLRendererInterface2D(const C_GLRendererInterface2D& other) = delete;
	explicit C_GLRendererInterface2D(C_GLRendererInterface2D&& other) noexcept
	{
		m_VAOid			  = other.m_VAOid;
		m_PlaneVertHandle = other.m_PlaneVertHandle;
	}
	~C_GLRendererInterface2D() override;

	void Render(const Renderer::RenderCall2D& call) override;

private:
	GLuint							   m_VAOid; // one generic buffer for all calls to ignore VAOs
	Renderer::Handle<Renderer::Buffer> m_PlaneVertHandle;
	Renderer::Handle<Renderer::Buffer> m_UVVertHandle;
	glm::uvec2						   m_RenderTargetSize;
};
} // namespace GLEngine::GLRenderer
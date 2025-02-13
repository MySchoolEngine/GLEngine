#pragma once

#include <Renderer/IRenderCommand.h>


namespace GLEngine::GLRenderer::Commands {

class C_GLClearColor final : public Renderer::I_RenderCommand {
public:
	explicit C_GLClearColor(glm::vec4&& color);
	explicit C_GLClearColor(glm::vec3 color);

	//===========================================
	// Renderer::I_RenderCommand
	//===========================================
	void					  Commit() override;
	E_Type					  GetType() const override;
	[[nodiscard]] std::string GetDescriptor() const override;

protected:
	glm::vec4 m_Colour;
};

} // namespace GLEngine::GLRenderer::Commands
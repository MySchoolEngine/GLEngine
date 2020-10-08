#pragma once

#include <Renderer/IRenderCommand.h>


namespace GLEngine::GLRenderer::Commands {

class C_GLClearColor : public Renderer::I_RenderCommand {
public:
	explicit C_GLClearColor(glm::vec4&& color);
	explicit C_GLClearColor(glm::vec3 color);

	//===========================================
	// Renderer::I_RenderCommand
	//===========================================
	virtual void Commit() override;
	virtual E_Type GetType() const override;
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override;
	[[nodiscard]] virtual std::string GetDescriptor() const override;

protected:
	glm::vec4 m_Colour;
};

}
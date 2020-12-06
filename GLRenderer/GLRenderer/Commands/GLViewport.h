#pragma once

#include <Renderer/IRenderCommand.h>


namespace GLEngine::GLRenderer::Commands {
class C_GLViewport : public Renderer::I_RenderCommand {
public:
	C_GLViewport(int x, int y, int widht, int height);
	C_GLViewport(glm::ivec2 coord, int widht, int height);

	//======================================
	// Renderer::I_RenderCommand
	//======================================
	virtual void Commit() override;
	virtual E_Type GetType() const override;
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override;
	[[nodiscard]] virtual std::string GetDescriptor() const override;

private:
	glm::ivec2 m_coord;
	glm::ivec2 m_Dimensions;
};
}
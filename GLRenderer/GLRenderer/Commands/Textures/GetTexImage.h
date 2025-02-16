#pragma once

#include <Renderer/IRenderCommand.h>

namespace GLEngine::Renderer {
class I_TextureViewStorage;
}

namespace GLEngine::GLRenderer::Commands {

class C_GetTexImage : public Renderer::I_RenderCommand {
public:
	explicit C_GetTexImage(std::promise<std::unique_ptr<Renderer::I_TextureViewStorage>>&& prom,
						   GLenum														   target,
						   int															   level,
						   GLenum														   format,
						   GLenum														   type,
						   std::size_t													   width,
						   std::size_t													   height,
						   std::uint8_t													   channels);

	//=================================================================================
	// Renderer::I_RenderCommand
	//=================================================================================
	void								  Commit() override;
	E_Type								  GetType() const override;

	//=================================================================================
	[[nodiscard]] virtual std::string GetDescriptor() const override;

private:
	std::promise<std::unique_ptr<Renderer::I_TextureViewStorage>> m_Promise;
	GLenum														  m_Target;
	int															  m_Level;
	GLenum														  m_Format;
	GLenum														  m_Type;
	std::size_t													  m_Width;
	std::size_t													  m_Height;
	std::uint8_t												  m_Channels;
};

} // namespace GLEngine::GLRenderer::Commands

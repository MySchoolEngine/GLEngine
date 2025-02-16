#pragma once

#include <Renderer/IRenderCommand.h>

namespace GLEngine::GLRenderer::Commands {

class C_GLCullFace final : public Renderer::I_RenderCommand {
public:
	enum class E_FaceMode : std::uint8_t {
		Front,
		Back,
		FrontAndBack,
		Last = FrontAndBack,
	};

	explicit C_GLCullFace(E_FaceMode mode);

	//===========================================
	// Renderer::I_RenderCommand
	//===========================================
	void					  Commit() override;
	E_Type					  GetType() const override;
	[[nodiscard]] std::string GetDescriptor() const override;

protected:
	E_FaceMode m_Mode;

	static GLenum EnumCast(E_FaceMode);
};

} // namespace GLEngine::GLRenderer::Commands
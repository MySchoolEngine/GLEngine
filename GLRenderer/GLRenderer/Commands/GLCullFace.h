#pragma once

#include <Renderer/IRenderCommand.h>

namespace GLEngine::GLRenderer::Commands {

class C_GLCullFace : public Renderer::I_RenderCommand {
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
	virtual void Commit() override;
	virtual E_Type GetType() const override;
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override;
	[[nodiscard]] std::string GetDescriptor() const override;

protected:
	E_FaceMode m_Mode;

	static GLenum EnumCast(E_FaceMode);
};

}
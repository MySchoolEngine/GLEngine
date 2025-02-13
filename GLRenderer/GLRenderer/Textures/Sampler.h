#pragma once

#include <Renderer/Textures/TextureDefinitions.h>
#include <Renderer/Textures/TextureSampler.h>

namespace GLEngine::GLRenderer {

class C_Sampler2D : public Renderer::I_TextureSampler2D {
public:
	C_Sampler2D(const Renderer::SamplerDescriptor2D& desc);
	~C_Sampler2D() override = default;

	void SetWrap(Renderer::E_WrapFunction wrapS, Renderer::E_WrapFunction wrapT) override;
	void SetFilter(Renderer::E_TextureFilter min, Renderer::E_TextureFilter mag) override;
	void SetBorderColor(const glm::vec4& color) override;

	
	void SetParameter(GLenum pname, const glm::vec4& value);
	void SetParameter(GLenum pname, GLint value);

private:
	GLuint m_Sampler;

	friend class C_GLDevice;
	friend class Textures::C_Texture;
};

} // namespace GLEngine::GLRenderer
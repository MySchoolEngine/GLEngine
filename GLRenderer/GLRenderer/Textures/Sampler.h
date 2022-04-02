#pragma once

#include <Renderer/Textures/TextureDefinitions.h>
#include <Renderer/Textures/TextureSampler.h>

namespace GLEngine::GLRenderer {

class C_Sampler2D : public Renderer::I_TextureSampler2D {
public:
	C_Sampler2D(const Renderer::SamplerDescriptor2D& desc)
		: Renderer::I_TextureSampler2D(desc)
	{
	}

	virtual void SetWrap(Renderer::E_WrapFunction wrapS, Renderer::E_WrapFunction wrapT) override;
	virtual void SetFilter(Renderer::E_TextureFilter min, Renderer::E_TextureFilter mag) override;
	virtual void SetBorderColor(const glm::vec4& color) override;

	
	void SetParameter(GLenum pname, const glm::vec4& value);
	void SetParameter(GLenum pname, GLint value);

private:
	GLuint m_Sampler;

	friend class C_GLDevice;
};

} // namespace GLEngine::GLRenderer
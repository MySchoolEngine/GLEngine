#include <GLRendererStdafx.h>

#include <GLRenderer/Textures/Sampler.h>

#include <glm/gtc/type_ptr.hpp>

namespace GLEngine::GLRenderer {
//=================================================================================
C_Sampler2D::C_Sampler2D(const Renderer::SamplerDescriptor2D& desc)
	: Renderer::I_TextureSampler2D(desc)
	, m_Sampler(0)
{
}

//=================================================================================
void C_Sampler2D::SetParameter(GLenum pname, GLint value)
{
	glSamplerParameteri(m_Sampler, pname, value);
}

//=================================================================================
void C_Sampler2D::SetParameter(GLenum pname, const glm::vec4& value)
{
	glSamplerParameterfv(m_Sampler, pname, glm::value_ptr(value));
}

//=================================================================================
void C_Sampler2D::SetWrap(Renderer::E_WrapFunction wrapS, Renderer::E_WrapFunction wrapT)
{
	SetParameter(GL_TEXTURE_WRAP_S, WrapFunctionToEnum(wrapS));
	SetParameter(GL_TEXTURE_WRAP_T, WrapFunctionToEnum(wrapT));
}

//=================================================================================
void C_Sampler2D::SetFilter(Renderer::E_TextureFilter min, Renderer::E_TextureFilter mag)
{
	SetParameter(GL_TEXTURE_MIN_FILTER, MinMagFilterToEnum(min));
	SetParameter(GL_TEXTURE_MAG_FILTER, MinMagFilterToEnum(mag));
}

//=================================================================================
void C_Sampler2D::SetBorderColor(const glm::vec4& color)
{
	SetParameter(GL_TEXTURE_BORDER_COLOR, color);
}

}
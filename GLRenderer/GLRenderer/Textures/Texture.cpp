#include <GLRendererStdafx.h>

#include <GLRenderer/Textures/Texture.h>

#include <GLRenderer/Commands/Textures/GLMakeTextureHandleResident.h>
#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>
#include <GLRenderer/Textures/TextureUtils.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Textures/TextureView.h>
#include <Renderer/Mesh/Scene.h>

#include <Core/Application.h>

#include <glm/gtc/type_ptr.hpp>

namespace GLEngine::GLRenderer::Textures {

//=================================================================================
C_Texture::C_Texture(const std::string& name, GLenum target)
	: m_bGroupOperations(false)
	, m_target(target)
	, m_texture(0)
	, m_Channels(0)
{
	glGenTextures(1, &m_texture);
	bind();
	glObjectLabel(GL_TEXTURE, m_texture, static_cast<GLsizei>(name.length()), name.c_str());
	unbind();
}

//=================================================================================
C_Texture::C_Texture(C_Texture&& t)
{
	if (&t == this)
	{
		return;
	}
	Clean();

	m_texture = t.m_texture;
	t.m_texture = 0;
	m_target = t.m_target;
	m_bGroupOperations = t.m_bGroupOperations;
	m_Handle = t.m_Handle;
	m_Dimensions = t.m_Dimensions;
	m_Channels = t.m_Channels;
}

//=================================================================================
void C_Texture::operator=(C_Texture&& rhs)
{
	if (&rhs == this)
	{
		return;
	}
	Clean();

	m_texture = rhs.m_texture;
	rhs.m_texture = 0;
	m_target = rhs.m_target;
	m_bGroupOperations = rhs.m_bGroupOperations;
	m_Handle = rhs.m_Handle;
	m_Dimensions = rhs.m_Dimensions;
	m_Channels = rhs.m_Channels;
}

//=================================================================================
C_Texture::~C_Texture()
{
	Clean();
}

//=================================================================================
void C_Texture::Clean()
{
	if (m_texture != 0)
		glDeleteTextures(1, &m_texture);
	m_texture = 0;
}

//=================================================================================
void C_Texture::bind() const
{
	if (!m_bGroupOperations)
		glBindTexture(m_target, m_texture);
}

//=================================================================================
void C_Texture::unbind() const
{
	if (!m_bGroupOperations)
		glBindTexture(m_target, 0);
}

//=================================================================================
void C_Texture::SetWrap(E_WrapFunction wrapS, E_WrapFunction wrapT)
{
	bind();
	SetTexParameter(GL_TEXTURE_WRAP_S, WrapFunctionToEnum(wrapS));
	SetTexParameter(GL_TEXTURE_WRAP_T, WrapFunctionToEnum(wrapT));
	unbind();
}

//=================================================================================
void C_Texture::SetWrap(E_WrapFunction wrapS, E_WrapFunction wrapT, E_WrapFunction wrapR)
{
	bind();
	SetTexParameter(GL_TEXTURE_WRAP_S, WrapFunctionToEnum(wrapS));
	SetTexParameter(GL_TEXTURE_WRAP_T, WrapFunctionToEnum(wrapT));
	SetTexParameter(GL_TEXTURE_WRAP_R, WrapFunctionToEnum(wrapR));
	unbind();
}

//=================================================================================
void C_Texture::SetFilter(GLint min, GLint mag)
{
	bind();
	SetTexParameter(GL_TEXTURE_MIN_FILTER, min);
	SetTexParameter(GL_TEXTURE_MAG_FILTER, mag);
	unbind();
}

//=================================================================================
void C_Texture::SetTexParameter(GLenum pname, const glm::vec4& value)
{
	bind();
	glTexParameterfv(m_target, pname, glm::value_ptr(value));
	unbind();
}

//=================================================================================
void C_Texture::SetTexParameter(GLenum pname, GLint value)
{
	bind();
	glTexParameteri(m_target, pname, value);
	unbind();
}

//=================================================================================
void C_Texture::GenerateMipMaps()
{
	bind();
	glGenerateMipmap(m_target);
	unbind();
}

//=================================================================================
void C_Texture::SetTexData2D(int level, const Renderer::MeshData::Texture& tex)
{
	SetDimensions({ tex.width, tex.height });
	m_Channels = 3;
	glTexImage2D(m_target,
		level,
		GL_RGB,
		(GLsizei)tex.width,
		(GLsizei)tex.height,
		0,
		GL_RGBA,
		T_TypeToGL<decltype(tex.data)::element_type>::value,
		tex.data.get());
}

//=================================================================================
void C_Texture::SetTexData2D(int level, const Renderer::I_TextureViewStorage* tex)
{
	SetDimensions(tex->GetDimensions());
	m_Channels = tex->GetNumElements();

	glTexImage2D(m_target,
		level,
		GetInternalFormat(),
		tex->GetDimensions().x,
		tex->GetDimensions().y,
		0,
		GetFormat(tex->GetChannels()),
		GL_UNSIGNED_BYTE,
		tex->GetData());
}

//=================================================================================
void C_Texture::SetInternalFormat(GLint internalFormat, GLint format, GLenum type)
{
	glTexImage2D(m_target,
		0,
		internalFormat,
		GetWidth(),
		GetHeight(),
		0,
		format,
		type, nullptr);
}

//=================================================================================
std::uint64_t C_Texture::CreateHandle()
{
	m_Handle = glGetTextureHandleARB(m_texture);
	return GetHandle();
}

//=================================================================================
std::uint64_t C_Texture::GetHandle() const
{
	return m_Handle;
}

//=================================================================================
void C_Texture::MakeHandleResident(bool val)
{
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::C_GLMakeTextureHandleResident>(m_Handle, val)
		)
	);
}

//=================================================================================
//=================================================================================
GLint C_Texture::GetInternalFormat() const
{
	switch (m_Channels)
	{
	case 1: return GL_RED;
	case 2: return GL_RG;
	case 3: return GL_RGB;
	case 4: return GL_RGBA;
	default:
		CORE_LOG(E_Level::Error, E_Context::Render, "Unknown number of elements: {}", m_Channels);
		break;
	}
}

}
#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/Textures/GLMakeTextureHandleResident.h>
#include <GLRenderer/Commands/Textures/GetTexImage.h>
#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>
#include <GLRenderer/Helpers/TextureHelpers.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureUtils.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/Textures/TextureStorage.h>
#include <Renderer/Textures/TextureView.h>

#include <Core/Application.h>

#include <glm/gtc/type_ptr.hpp>

namespace GLEngine::GLRenderer::Textures {

//=================================================================================
C_Texture::C_Texture(const std::string& name, GLenum target)
	: Renderer::I_DeviceTexture({})
	, m_bGroupOperations(false)
	, m_target(target)
	, m_texture(0)
	, m_Handle(0)
	, m_Format(Renderer::E_TextureFormat::RGBA8i)
{
	m_Desc.name = name;
	glGenTextures(1, &m_texture);
	bind();
	glObjectLabel(GL_TEXTURE, m_texture, static_cast<GLsizei>(name.length()), name.c_str());
	unbind();
}

//=================================================================================
C_Texture::C_Texture(C_Texture&& t)
	: Renderer::I_DeviceTexture({})
{
	if (&t == this)
	{
		return;
	}
	Clean();

	m_texture		   = t.m_texture;
	t.m_texture		   = 0;
	m_target		   = t.m_target;
	m_bGroupOperations = t.m_bGroupOperations;
	m_Handle		   = t.m_Handle;
	m_Format		   = t.m_Format;
}

//=================================================================================
C_Texture::C_Texture(const Renderer::TextureDescriptor& desc)
	: Renderer::I_DeviceTexture(desc)
{
}

//=================================================================================
void C_Texture::operator=(C_Texture&& rhs)
{
	if (&rhs == this)
	{
		return;
	}
	Clean();

	m_texture		   = rhs.m_texture;
	rhs.m_texture	   = 0;
	m_target		   = rhs.m_target;
	m_bGroupOperations = rhs.m_bGroupOperations;
	m_Handle		   = rhs.m_Handle;
	m_Format		   = rhs.m_Format;
}

//=================================================================================
C_Texture::~C_Texture()
{
	CORE_LOG(E_Level::Debug, E_Context::Render, "Texture being deleted name: {}", m_Desc.name);
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
void C_Texture::SetWrap(Renderer::E_WrapFunction wrapS, Renderer::E_WrapFunction wrapT)
{
	bind();
	SetTexParameter(GL_TEXTURE_WRAP_S, WrapFunctionToEnum(wrapS));
	SetTexParameter(GL_TEXTURE_WRAP_T, WrapFunctionToEnum(wrapT));
	unbind();
}

//=================================================================================
void C_Texture::SetWrap(Renderer::E_WrapFunction wrapS, Renderer::E_WrapFunction wrapT, Renderer::E_WrapFunction wrapR)
{
	StartGroupOp();
	SetTexParameter(GL_TEXTURE_WRAP_S, WrapFunctionToEnum(wrapS));
	SetTexParameter(GL_TEXTURE_WRAP_T, WrapFunctionToEnum(wrapT));
	SetTexParameter(GL_TEXTURE_WRAP_R, WrapFunctionToEnum(wrapR));
	EndGroupOp();
}

//=================================================================================
void C_Texture::SetBorderColor(const glm::vec4& color)
{
	bind();
	SetTexParameter(GL_TEXTURE_BORDER_COLOR, color);
	unbind();
}

//=================================================================================
void C_Texture::SetFilter(Renderer::E_TextureFilter min, Renderer::E_TextureFilter mag)
{
	StartGroupOp();
	SetTexParameter(GL_TEXTURE_MIN_FILTER, MinMagFilterToEnum(min));
	SetTexParameter(GL_TEXTURE_MAG_FILTER, MinMagFilterToEnum(mag));
	EndGroupOp();
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
	bind();
	SetDimensions({tex.width, tex.height});
	static_assert(std::is_same_v<std::uint8_t, decltype(tex.data)::element_type>, "Format have been changed.");
	m_Desc.format = Renderer::E_TextureFormat::RGBA8i;

	glTexImage2D(m_target, level,
				 GetOpenGLInternalFormat(m_Desc.format),  // internal format
				 (GLsizei)tex.width, (GLsizei)tex.height, // dimensions
				 0,										  // border
				 GL_RGBA,								  // format
				 T_TypeToGL<decltype(tex.data)::element_type>::value,
				 tex.data.get() // data
	);
	unbind();
}

//=================================================================================
void C_Texture::SetTexData2D(int level, const Renderer::I_TextureViewStorage* tex)
{
	bind();
	SetDimensions(tex->GetDimensions());

	glTexImage2D(m_target, level,
				 GetOpenGLInternalFormat(m_Desc.format), // internal format
				 tex->GetDimensions().x, tex->GetDimensions().y,
				 0,								// border
				 GetFormat(tex->GetChannels()), // format
				 GetUnderlyingType(tex),		// TODO
				 tex->GetData());				// data
	unbind();
}

//=================================================================================
void C_Texture::SetTexData2D(int level, const Renderer::C_TextureView tex)
{
	StartGroupOp();
	SetTexData2D(level, tex.GetStorage());
	if (tex.UseBorderColor())
	{
		SetBorderColor(tex.GetBorderColor<glm::vec4>());
	}
	SetWrap(tex.GetWrapFunction(), tex.GetWrapFunction());
	EndGroupOp();
}

//=================================================================================
void C_Texture::SetInternalFormat(Renderer::E_TextureFormat internalFormat, GLint format)
{
	bind();
	m_Desc.format = internalFormat;
	glTexImage2D(m_target,
				 0,									// level
				 GetOpenGLInternalFormat(m_Desc.format), // internal format
				 GetWidth(), GetHeight(),			// dimensions
				 0,									// border
				 format,							// this should be deduced from m_Format too
				 OpenGLUnderlyingType(m_Desc.format),
				 nullptr); // no data passed as we just want to allocate buffer
	unbind();
}

//=================================================================================
std::uint64_t C_Texture::CreateHandle()
{
	if (m_Handle == 0)
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
	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::C_GLMakeTextureHandleResident>(m_Handle, val));
}

//=================================================================================
T_TexBufferFuture C_Texture::GetTextureData() const
{
	std::promise<std::unique_ptr<Renderer::I_TextureViewStorage>> promise;

	auto  ret	   = promise.get_future();
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	renderer.AddCommand(std::make_unique<Commands::C_GetTexImage>(std::move(promise), m_target,
																  0, // level
																  GetOpenGLInternalFormat(m_Desc.format),
																  T_TypeToGL<std::uint8_t>::value, // todo
																  GetWidth(), GetHeight(),		   // resolution
																  Renderer::GetNumberChannels(m_Desc.format)));
	return ret;
}

//=================================================================================
void* C_Texture::GetDeviceTextureHandle() const
{
	return (void*)(intptr_t)(m_texture);
}

//=================================================================================
bool C_Texture::IsAllocated() const
{
	return m_texture;
}

} // namespace GLEngine::GLRenderer::Textures
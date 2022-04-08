#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/Textures/GLMakeTextureHandleResident.h>
#include <GLRenderer/Commands/Textures/GetTexImage.h>
#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>
#include <GLRenderer/Helpers/TextureHelpers.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureManager.h>
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
	, m_DefaultSampler({})
	, m_bGroupOperations(false)
	, m_texture(0)
	, m_Handle(0)
	, m_Format(Renderer::E_TextureFormat::RGBA8i)
{
	m_Desc.name = name;
	m_Desc.type = Renderer::E_TextureType::TEXTUE_2D; // TODO
	glGenTextures(1, &m_texture);
	CORE_LOG(E_Level::Error, E_Context::Render, "Texture {}", m_texture);
	bind();
	glObjectLabel(GL_TEXTURE, m_texture, static_cast<GLsizei>(name.length()), name.c_str());
	unbind();

	SetInternalFormat(m_Format, GetOpenGLFormat(m_Format));
}

//=================================================================================
C_Texture::C_Texture(C_Texture&& t)
	: Renderer::I_DeviceTexture({})
	, m_DefaultSampler(t.m_DefaultSampler)
{
	if (&t == this)
	{
		return;
	}
	Clean();

	m_texture		   = t.m_texture;
	t.m_texture		   = 0;
	m_bGroupOperations = t.m_bGroupOperations;
	m_Handle		   = t.m_Handle;
	m_Format		   = t.m_Format;
}

//=================================================================================
C_Texture::C_Texture(const Renderer::TextureDescriptor& desc)
	: Renderer::I_DeviceTexture(desc)
	, m_bGroupOperations(false)
	, m_Handle(0)
	, m_DefaultSampler({})
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
	if (!m_bGroupOperations && !m_bIsTexture)
		glBindTexture(GetTextureType(m_Desc.type), m_texture);
}

//=================================================================================
void C_Texture::unbind() const
{
	if (!m_bGroupOperations && !m_bIsTexture)
		glBindTexture(GetTextureType(m_Desc.type), 0);
}

//=================================================================================
void C_Texture::SetWrap(Renderer::E_WrapFunction wrapS, Renderer::E_WrapFunction wrapT)
{
	if (!m_bIsTexture)
	{
		bind();
		SetTexParameter(GL_TEXTURE_WRAP_S, WrapFunctionToEnum(wrapS));
		SetTexParameter(GL_TEXTURE_WRAP_T, WrapFunctionToEnum(wrapT));
		unbind();
	}
	else
	{
		m_DefaultSampler.SetWrap(wrapS, wrapT);
	}
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
	if (!m_bIsTexture)
	{
		bind();
		SetTexParameter(GL_TEXTURE_BORDER_COLOR, color);
		unbind();
	}
	else
	{
		m_DefaultSampler.SetBorderColor(color);
	}
}

//=================================================================================
void C_Texture::SetFilter(Renderer::E_TextureFilter min, Renderer::E_TextureFilter mag)
{
	if (!m_bIsTexture)
	{
		StartGroupOp();
		SetTexParameter(GL_TEXTURE_MIN_FILTER, MinMagFilterToEnum(min));
		SetTexParameter(GL_TEXTURE_MAG_FILTER, MinMagFilterToEnum(mag));
		EndGroupOp();
	}
	else
	{
		m_DefaultSampler.SetFilter(min, mag);
	}
}

//=================================================================================
void C_Texture::SetTexParameter(GLenum pname, const glm::vec4& value)
{
	bind();
	if (!m_bIsTexture)
		glTexParameterfv(GetTextureType(m_Desc.type), pname, glm::value_ptr(value));
	else
		glTextureParameterfv(m_texture, pname, glm::value_ptr(value));
	unbind();
}

//=================================================================================
void C_Texture::SetTexParameter(GLenum pname, GLint value)
{
	bind();
	if (!m_bIsTexture)
		glTexParameteri(GetTextureType(m_Desc.type), pname, value);
	else
		glTextureParameteri(m_texture, pname, value);
	unbind();
}

//=================================================================================
void C_Texture::GenerateMipMaps()
{
	bind();
	if (!m_bIsTexture)
		glGenerateMipmap(GetTextureType(m_Desc.type));
	else
		glGenerateTextureMipmap(m_texture);
	unbind();
}

//=================================================================================
void C_Texture::SetTexData2D(int level, const Renderer::I_TextureViewStorage* tex)
{
	GLE_ASSERT(tex, "This should be smth like reference");
	if (!m_bIsTexture)
	{
		bind();
		SetDimensions(tex->GetDimensions());

		glTexImage2D(GetTextureType(m_Desc.type), level,
					 GetOpenGLInternalFormat(m_Desc.format), // internal format
					 tex->GetDimensions().x, tex->GetDimensions().y,
					 0,								// border
					 GetFormat(tex->GetChannels(), Renderer::IsIntegral(tex->GetStorageType())), // format
					 GetUnderlyingType(tex),		// TODO
					 tex->GetData());															 // data
		unbind();
	}
	else
	{
		// https://www.khronos.org/registry/OpenGL/specs/gl/glspec45.compatibility.pdf Table 8.35
		glTextureSubImage2D(m_texture,
							0,												//!< Level
							0, 0,											//!< Offsets
							tex->GetDimensions().x, tex->GetDimensions().y, //!< Dimmensions
							GetFormat(tex->GetChannels(), Renderer::IsIntegral(tex->GetStorageType())),
							GetUnderlyingType(tex),
							tex->GetData());
	}
	// automatic mip-maps generation
	if (m_Desc.m_Levels > 1)
		glGenerateTextureMipmap(m_texture);

	SetReadyToUse();
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
	if (!m_bIsTexture)
	{
		// IMHO I don't want to do that with my immutable textures
		bind();
		m_Desc.format = internalFormat;
		glTexImage2D(GetTextureType(m_Desc.type),
					 0,									// level
					 GetOpenGLInternalFormat(m_Desc.format), // internal format
					 GetWidth(), GetHeight(),			// dimensions
					 0,									// border
					 format,							// this should be deduced from m_Format too
					 OpenGLUnderlyingType(m_Desc.format),
					 nullptr); // no data passed as we just want to allocate buffer
		unbind();
	}
}

//=================================================================================
std::uint64_t C_Texture::CreateHandle()
{
	ErrorCheck();
	if (m_IsPresentOnGPU && m_Handle == 0)
	{
		if (!m_bIsTexture)
		  m_Handle = glGetTextureHandleARB(m_texture);
		else
			m_Handle = glGetTextureSamplerHandleARB(m_texture, m_DefaultSampler.m_Sampler);
	}
	ErrorCheck();
	return GetHandle();
}

//=================================================================================
std::uint64_t C_Texture::GetHandle() const
{
	if (!m_IsPresentOnGPU)
	{
		return C_TextureManager::Instance().GetErrorTexture()->CreateHandle();
	}
	return m_Handle;
}

//=================================================================================
void C_Texture::MakeHandleResident(bool val)
{
	if (!m_IsPresentOnGPU)
	{
		C_TextureManager::Instance().GetErrorTexture()->MakeHandleResident(val);
		return;
	}
	if (m_IsResidient!=val)
		Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::C_GLMakeTextureHandleResident>(GetHandle(), val));
	m_IsResidient = val;
}

//=================================================================================
T_TexBufferFuture C_Texture::GetTextureData() const
{
	std::promise<std::unique_ptr<Renderer::I_TextureViewStorage>> promise;

	auto  ret	   = promise.get_future();
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	renderer.AddCommand(std::make_unique<Commands::C_GetTexImage>(std::move(promise), GetTextureType(m_Desc.type),
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
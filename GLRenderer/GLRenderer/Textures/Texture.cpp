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

RTTR_REGISTRATION
{
	using namespace GLEngine::GLRenderer::Textures;

	rttr::registration::class_<C_Texture>("C_Texture").constructor<const GLEngine::Renderer::TextureDescriptor&>();
		//.property("Name", &GLEngine::Renderer::I_DeviceTexture::m_Desc::name);

	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<C_Texture>>();
}

namespace GLEngine::GLRenderer::Textures {

//=================================================================================
C_Texture::C_Texture(const Renderer::TextureDescriptor& desc)
	: Renderer::I_DeviceTexture(desc)
	, m_Handle(0)
	, m_DefaultSampler({})
{
}

//=================================================================================
C_Texture::C_Texture(C_Texture&& t)
	: Renderer::I_DeviceTexture(std::move(t))
	, m_DefaultSampler(t.m_DefaultSampler)
{
	GLE_ASSERT(m_texture == 0, "Moving into the texture that haven't been cleared before. This will leak memory.");
	if (&t == this)
	{
		return;
	}
	Clean();

	m_texture	= t.m_texture;
	t.m_texture = 0;
	m_Handle	= t.m_Handle;
}

//=================================================================================
void C_Texture::operator=(C_Texture&& rhs)
{
	if (&rhs == this)
	{
		return;
	}
	Clean();

	m_texture	  = rhs.m_texture;
	rhs.m_texture = 0;
	m_Handle	  = rhs.m_Handle;
	m_Desc		  = rhs.m_Desc;
}

//=================================================================================
C_Texture::~C_Texture()
{
	CORE_LOG(E_Level::Debug, E_Context::Render, "Texture being deleted name: {}", m_Desc.name);
	GLE_ASSERT(m_texture == 0, "Descruting the texture that haven't been cleared before. This will leak memory.");
	Clean();
}

//=================================================================================
void C_Texture::Clean()
{
	m_texture = 0;
}

//=================================================================================
void C_Texture::bind() const
{
	glBindTexture(GetTextureType(m_Desc.type), m_texture);
}

//=================================================================================
void C_Texture::unbind() const
{
	glBindTexture(GetTextureType(m_Desc.type), 0);
}

//=================================================================================
void C_Texture::SetWrap(Renderer::E_WrapFunction wrapS, Renderer::E_WrapFunction wrapT)
{
	m_DefaultSampler.SetWrap(wrapS, wrapT);
}

//=================================================================================
void C_Texture::SetWrap(Renderer::E_WrapFunction wrapS, Renderer::E_WrapFunction wrapT, Renderer::E_WrapFunction wrapR)
{
	SetParameter(GL_TEXTURE_WRAP_S, WrapFunctionToEnum(wrapS));
	SetParameter(GL_TEXTURE_WRAP_T, WrapFunctionToEnum(wrapT));
	SetParameter(GL_TEXTURE_WRAP_R, WrapFunctionToEnum(wrapR));
}

//=================================================================================
void C_Texture::SetBorderColor(const glm::vec4& color)
{
	m_DefaultSampler.SetBorderColor(color);
}

//=================================================================================
void C_Texture::SetFilter(Renderer::E_TextureFilter min, Renderer::E_TextureFilter mag)
{
	m_DefaultSampler.SetFilter(min, mag);
}

//=================================================================================
void C_Texture::SetParameter(GLenum pname, const glm::vec4& value)
{
	glTextureParameterfv(m_texture, pname, glm::value_ptr(value));
}

//=================================================================================
void C_Texture::SetParameter(GLenum pname, GLint value)
{
	glTextureParameteri(m_texture, pname, value);
}

//=================================================================================
void C_Texture::GenerateMipMaps()
{
	glGenerateTextureMipmap(m_texture);
}

//=================================================================================
void C_Texture::SetTexData2D(int level, const Renderer::I_TextureViewStorage* tex)
{
	GLE_ASSERT(tex, "This should be smth like reference");
	// https://www.khronos.org/registry/OpenGL/specs/gl/glspec45.compatibility.pdf Table 8.35
	glTextureSubImage2D(m_texture,
						0,												//!< Level
						0, 0,											//!< Offsets
						tex->GetDimensions().x, tex->GetDimensions().y, //!< Dimmensions
						GetFormat(tex->GetChannels(), 
						Renderer::IsIntegral(tex->GetStorageType())), 
						GetUnderlyingType(tex), tex->GetData());

	// automatic mip-maps generation
	if (m_Desc.m_Levels > 1)
		glGenerateTextureMipmap(m_texture);

	SetReadyToUse();
}

//=================================================================================
void C_Texture::SetTexData2D(int level, const Renderer::C_TextureView tex)
{
	SetTexData2D(level, tex.GetStorage());
	if (tex.UseBorderColor())
	{
		SetBorderColor(tex.GetBorderColor<glm::vec4>());
	}
	SetWrap(tex.GetWrapFunction(), tex.GetWrapFunction());
}

//=================================================================================
std::uint64_t C_Texture::CreateHandle()
{
	if (m_IsPresentOnGPU && m_Handle == 0)
	{
		m_Handle = glGetTextureSamplerHandleARB(m_texture, m_DefaultSampler.m_Sampler);
	}
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
	if (m_IsResidient != val)
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
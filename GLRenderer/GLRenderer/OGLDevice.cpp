#include <GLRendererStdafx.h>

#include <GLRenderer/Helpers/TextureHelpers.h>
#include <GLRenderer/OGLDevice.h>
#include <GLRenderer/Textures/Sampler.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/FBO/Framebuffer.h>

#include <Renderer/Textures/DeviceTexture.h>
#include <Renderer/Textures/TextureDefinitions.h>

#include <limits>

namespace GLEngine::GLRenderer {
namespace NVX_ext {
constexpr static auto GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX		   = 0x9047;
constexpr static auto GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX   = 0x9048;
constexpr static auto GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX = 0x9049;
constexpr static auto GPU_MEMORY_INFO_EVICTION_COUNT_NVX		   = 0x904A;
constexpr static auto GPU_MEMORY_INFO_EVICTED_MEMORY_NVX		   = 0x904B;
} // namespace NVX_ext

//=================================================================================
C_GLDevice::C_GLDevice()
	: m_TotalMemory(std::numeric_limits<int>::max())
	, m_TextureMemoryUsed(0)
{
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if (status == 0)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "GLFW: Glad wasn't loaded properlly. Status {}", status);
	}

	const GLubyte* vendor	= glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version	= glGetString(GL_VERSION);

	const auto maxLen = strlen(reinterpret_cast<const char*>(renderer)) + 10;

	CORE_LOG(E_Level::Info, E_Context::Render, "{:=>{}}", "=", maxLen);
	CORE_LOG(E_Level::Info, E_Context::Render, "Vendor {}", vendor);
	CORE_LOG(E_Level::Info, E_Context::Render, "renderer {}", renderer);
	CORE_LOG(E_Level::Info, E_Context::Render, "version {}", version);
	CORE_LOG(E_Level::Info, E_Context::Render, "{:=>{}}", "=", maxLen);

	if (HasExtension("GL_NVX_gpu_memory_info"))
	{
		// https://www.khronos.org/registry/OpenGL/extensions/NVX/NVX_gpu_memory_info.txt
		GLint totalMemoryKb = 0;
		glGetIntegerv(NVX_ext::GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalMemoryKb);
		CORE_LOG(E_Level::Info, E_Context::Render, "Total memroy {}", totalMemoryKb);

		GLint currentMemoryKb = 0;
		glGetIntegerv(NVX_ext::GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &currentMemoryKb);
		m_TotalMemory = currentMemoryKb;
		CORE_LOG(E_Level::Info, E_Context::Render, "Memroy available {}", currentMemoryKb);
	}
	else
	{
		CORE_LOG(E_Level::Warning, E_Context::Render, "No memory estimate because of missing extension.");
	}

	GLint samples;
	glGetIntegerv(GL_MAX_SAMPLES, &samples);
	CORE_LOG(E_Level::Info, E_Context::Render, "Max sampels available {}", samples);

	// this should be max samples for mutisampled texture
}

//=================================================================================
bool C_GLDevice::AllocateTexture(Renderer::I_DeviceTexture& texture)
{
	auto* textureGL = reinterpret_cast<Textures::C_Texture*>(&texture);
	GLE_ASSERT(textureGL, "Wrong API texture passed in");

	// now I need to create new texture name, set it to the texture with possibility to invalidate it (GPUHandle reference?)
	// allocate the memory according to the descriptor store handle here and end function! Yaaaay!

	GLuint		texID;
	const auto& descriptor = texture.GetDescriptor();
	glCreateTextures(GetTextureType(descriptor.type), 1, &texID);
	glObjectLabel(GL_TEXTURE, texID, static_cast<GLsizei>(descriptor.name.length()), descriptor.name.c_str());

	std::size_t memory = 0;

	switch (descriptor.type)
	{
	case Renderer::E_TextureType::TEXTURE_2D: {
		glTextureStorage2D(texID, descriptor.m_Levels, GetOpenGLInternalFormat(descriptor.format), static_cast<GLsizei>(descriptor.width), static_cast<GLsizei>(descriptor.height));
		// still missing 1.33 for mip mapping and size of the type
		memory = descriptor.width * descriptor.height * Renderer::GetNumberChannels(descriptor.format) * SizeOfGLType(OpenGLUnderlyingType(descriptor.format));
		textureGL->SetGPUID(texID);
		m_TextureMemoryUsed += memory;

		textureGL->SetParameter(GL_TEXTURE_BASE_LEVEL, 0);
		textureGL->SetParameter(GL_TEXTURE_MAX_LEVEL, descriptor.m_Levels - 1);

		AllocateSampler(textureGL->m_DefaultSampler); // TODO once proper sampler system ready delete this
		return true;
	}
	case Renderer::E_TextureType::TEXTURE_2D_ARRAY: {
		glTextureStorage3D(texID, descriptor.m_Levels, GetOpenGLInternalFormat(descriptor.format), 
						   static_cast<GLsizei>(descriptor.width), static_cast<GLsizei>(descriptor.height),
						   static_cast<GLsizei>(descriptor.m_NumTextures));
		memory = descriptor.width * descriptor.height * Renderer::GetNumberChannels(descriptor.format) * SizeOfGLType(OpenGLUnderlyingType(descriptor.format))
				 * descriptor.m_NumTextures;
		textureGL->SetGPUID(texID);
		m_TextureMemoryUsed += memory;

		textureGL->SetParameter(GL_TEXTURE_BASE_LEVEL, 0);
		textureGL->SetParameter(GL_TEXTURE_MAX_LEVEL, descriptor.m_Levels - 1);

		AllocateSampler(textureGL->m_DefaultSampler); // TODO once proper sampler system ready delete this
		return true;
	}
	default:
		GLE_ASSERT(false, "Unimplemented case.");
		break;
	}

	glDeleteTextures(1, &texID);


	return false;
}

//=================================================================================
std::size_t C_GLDevice::GetAllocatedMemory() const
{
	return m_TextureMemoryUsed;
}

//=================================================================================
void C_GLDevice::DestroyTexture(Renderer::I_DeviceTexture& texture)
{
	auto* textureGL = reinterpret_cast<Textures::C_Texture*>(&texture);
	GLE_ASSERT(textureGL, "Wrong API texture passed in");
	auto tex = textureGL->GetTexture();
	glDeleteTextures(1, &tex);
	textureGL->m_texture = 0;
	DestroySampler(textureGL->m_DefaultSampler);
	textureGL->m_IsPresentOnGPU = false;
}

//=================================================================================
bool C_GLDevice::HasExtension(const std::string_view ext) const
{
	GLint count;
	glGetIntegerv(GL_NUM_EXTENSIONS, &count);

	for (GLint i = 0; i < count; ++i)
	{
		const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
		if (ext == extension)
			return true;
	}
	return false;
}

//=================================================================================
bool C_GLDevice::AllocateSampler(Renderer::I_TextureSampler2D& sampler)
{
	auto*  samplerGL = reinterpret_cast<C_Sampler2D*>(&sampler);
	GLuint samplerID;
	glCreateSamplers(1, &samplerID);

	samplerGL->m_Sampler = samplerID;
	return true;
}

//=================================================================================
void C_GLDevice::DestroySampler(Renderer::I_TextureSampler2D& texture)
{
	auto* samplerGL = reinterpret_cast<C_Sampler2D*>(&texture);
	glDeleteSamplers(1, &samplerGL->m_Sampler);
}

//=================================================================================
C_Framebuffer* C_GLDevice::AllocateFramebuffer(const std::string_view name)
{
	return new C_Framebuffer(name);
}

//=================================================================================
C_Framebuffer C_GLDevice::GetDefualtRendertarget()
{
	return C_Framebuffer("default", true);
}

} // namespace GLEngine::GLRenderer
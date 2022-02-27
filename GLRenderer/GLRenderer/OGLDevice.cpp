#include <GLRendererStdafx.h>

#include <GLRenderer/Helpers/TextureHelpers.h>
#include <GLRenderer/OGLDevice.h>
#include <GLRenderer/Textures/Texture.h>

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

	std::size_t memory = 0;

	switch (descriptor.type)
	{
	case Renderer::E_TextureType::TEXTUE_2D: {

		GLint currentMemoryKb = 0;
		glGetIntegerv(NVX_ext::GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &currentMemoryKb);
		CORE_LOG(E_Level::Info, E_Context::Render, "Memroy available {}", currentMemoryKb);
		glTextureStorage2D(texID, descriptor.m_Levels, GetOpenGLInternalFormat(descriptor.format), descriptor.width, descriptor.height);
		// still missing 1.33 for mip mapping and size of the type
		memory = descriptor.width * descriptor.height * Renderer::GetNumberChannels(descriptor.format);
		glGetIntegerv(NVX_ext::GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &currentMemoryKb);
		CORE_LOG(E_Level::Info, E_Context::Render, "Memroy available {}", currentMemoryKb);
		return true;
	}
	default:
		break;
	}


	return false;
}
// C_GLDevice::T_TextureHandle C_GLDevice::CreateTexture(const Renderer::TextureDescriptor& descriptor)
// {
// 	GLuint texture;
// 	glGenTextures(1, &texture);
// 	glTexImage2D(GetTextureType(descriptor.type),
// 				 0,											 // level
// 				 GetOpenGLInternalFormat(descriptor.format), // internal format
// 				 descriptor.width, descriptor.height,		 // dimensions
// 				 0,											 // border
// 				 GL_RGBA,									 // this should be deduced from m_Format too
// 				 OpenGLUnderlyingType(descriptor.format),
// 				 nullptr); // no data passed as we just want to allocate buffer
//
// 	return nullptr;
// }

//=================================================================================
std::size_t C_GLDevice::GetAllocatedMemory() const
{
	return 0;
}

//=================================================================================
void C_GLDevice::DestroyTexture(T_TextureHandle& texture)
{
	throw std::logic_error("The method or operation is not implemented.");
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

} // namespace GLEngine::GLRenderer
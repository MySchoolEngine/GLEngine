#include <GLRendererStdafx.h>

#include <GLRenderer/GLResourceManager.h>
#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>

namespace GLEngine::GLRenderer {

//=================================================================================
Renderer::Handle<Renderer::Texture> GLResourceManager::createTexture(const Renderer::TextureDescriptor& desc)
{
	return {};
	//	m_TexturePool.CreateNew(desc);
}

//=================================================================================
Renderer::Handle<Renderer::Buffer> GLResourceManager::createBuffer(const Renderer::BufferDescriptor& desc)
{
	auto handle = m_BufferPool.CreateNew(desc);
	if (auto* buffer = m_BufferPool.GetResource(handle))
	{
		const GLenum usage		= buffer->GetUsage(); // todo
		const GLenum bufferType = buffer->GetType();
		glGenBuffers(1, &buffer->m_id);
		glBindBuffer(bufferType, buffer->m_id);
		glBufferData(bufferType, desc.size, nullptr, usage);
		glBindBuffer(bufferType, 0);
		// todo mapping
		// https://www.cppstories.com/2015/01/persistent-mapped-buffers-in-opengl/
	}
	return handle;
}

//=================================================================================
void GLResourceManager::destroyBuffer(const Renderer::Handle<Renderer::Buffer>& handle)
{
	auto* buffer = m_BufferPool.GetResource(handle);
	GLE_ASSERT(buffer, "Resource already destroyed");
	glDeleteBuffers(1, &buffer->m_id);
	m_BufferPool.RemoveHandle(handle);
}

//=================================================================================
GLBuffer* GLResourceManager::GetBuffer(const Renderer::Handle<Renderer::Buffer>& handle)
{
	return m_BufferPool.GetResource(handle);
}

//=================================================================================
Shaders::C_ShaderProgram* GLResourceManager::GetShader(const Renderer::Handle<Renderer::Shader>& handle)
{
	return nullptr;
}

//=================================================================================
Renderer::Handle<Renderer::Shader> GLResourceManager::createShader(const std::filesystem::path& path)
{
	return {};
}

//=================================================================================
void GLResourceManager::destoryShader(Renderer::Handle<Renderer::Shader> handle)
{
}

//=================================================================================
void GLResourceManager::destoryTexture(Renderer::Handle<Renderer::Texture> handle)
{
}

//=================================================================================
Renderer::Handle<Renderer::Sampler> GLResourceManager::createSampler(const Renderer::SamplerDescriptor2D& desc)
{
	return {};
}

//=================================================================================
void GLResourceManager::destroySampler(const Renderer::Handle<Renderer::Sampler>& handle)
{
}

} // namespace GLEngine::GLRenderer

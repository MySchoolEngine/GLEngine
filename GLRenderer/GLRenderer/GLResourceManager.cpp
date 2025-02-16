#include <GLRendererStdafx.h>

#include <GLRenderer/GLResourceManager.h>
#include <GLRenderer/OGLDevice.h>

namespace GLEngine::GLRenderer {

//=================================================================================
Renderer::Handle<Renderer::Texture> GLResourceManager::createTexture(const Renderer::TextureDescriptor& desc)
{
	auto handle = m_TexturePool.CreateNew(desc);
	if (auto* texture = m_TexturePool.GetResource(handle))
	{
		m_Device->AllocateTexture(*texture);
	}
	return handle;
}

//=================================================================================
Renderer::Handle<Renderer::Buffer> GLResourceManager::createBuffer(const Renderer::BufferDescriptor& desc)
{
	auto handle = m_BufferPool.CreateNew(desc);
	if (auto* buffer = m_BufferPool.GetResource(handle))
	{
		const GLenum usage		= buffer->GetUsage(); // todo
		const GLenum bufferType = buffer->GetType();
		glGenBuffers(1, &buffer->m_ID);
		glBindBuffer(bufferType, buffer->m_ID);
		glBufferData(bufferType, desc.size, nullptr, usage);
		glBindBuffer(bufferType, 0);
		if (desc.name.empty())
		{
			glObjectLabel(GL_BUFFER, buffer->m_ID, static_cast<GLsizei>(desc.name.length()), desc.name.c_str());
		}
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
	glDeleteBuffers(1, &buffer->m_ID);
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
	if (auto* texture = m_TexturePool.GetResource(handle))
	{
		m_Device->DestroyTexture(*texture);
	}
	m_TexturePool.RemoveHandle(handle);
}

//=================================================================================
const Renderer::TextureDescriptor* GLResourceManager::getDescriptor(Renderer::Handle<Renderer::Texture> handle)
{
	if (auto* texture = GetTexture(handle))
	{
		return &texture->GetDescriptor();
	}
	return nullptr;
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

//=================================================================================
Renderer::Handle<Renderer::Pipeline> GLResourceManager::createPipeline(const Renderer::PipelineDescriptor& desc)
{
	// no caching so far, but needed as well as in Vulkan
	auto handle = m_PipelinePool.CreateNew(desc);
	return handle;
}

//=================================================================================
void GLResourceManager::destoryPipeline(Renderer::Handle<Renderer::Pipeline> handle)
{
	// no real object here, so no destruction
	m_PipelinePool.RemoveHandle(handle);
}

//=================================================================================
GLPipeline* GLResourceManager::GetPipeline(const Renderer::Handle<Renderer::Pipeline>& handle)
{
	return m_PipelinePool.GetResource(handle);
}

//=================================================================================
Textures::C_Texture* GLResourceManager::GetTexture(const Renderer::Handle<Renderer::Texture>& handle)
{
	return m_TexturePool.GetResource(handle);
}

//=================================================================================
void GLResourceManager::Init(C_GLDevice* device)
{
	m_Device = device;
}

//=================================================================================
// GLPipeline
//=================================================================================
GLPipeline::GLPipeline(const Renderer::PipelineDescriptor& desc)
	: m_Desc(desc)
{
}

//=================================================================================
const Renderer::PipelineDescriptor& GLPipeline::GetDesc() const
{
	return m_Desc;
}

} // namespace GLEngine::GLRenderer

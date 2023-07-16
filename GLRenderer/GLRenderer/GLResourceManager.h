#pragma once

#include <GLRenderer/Buffers/GLBuffer.h>
#include <GLRenderer/Shaders/ShaderProgram.h>
#include <GLRenderer/Textures/Texture.h>

#include <Renderer/Resources/ResourceManager.h>

namespace GLEngine::GLRenderer {

class GLPipeline {
public:
	GLPipeline(const Renderer::PipelineDescriptor& desc);
	const Renderer::PipelineDescriptor& GetDesc() const;

private:
	Renderer::PipelineDescriptor m_Desc;
};

class GLResourceManager : public Renderer::ResouceManager {
public:
	[[nodiscard]] virtual Renderer::Handle<Renderer::Shader>   createShader(const std::filesystem::path& path) override;
	virtual void											   destoryShader(Renderer::Handle<Renderer::Shader> handle) override;
	[[nodiscard]] virtual Renderer::Handle<Renderer::Pipeline> createPipeline(const Renderer::PipelineDescriptor& desc) override;
	virtual void											   destoryPipeline(Renderer::Handle<Renderer::Pipeline> handle) override;
	[[nodiscard]] virtual Renderer::Handle<Renderer::Texture>  createTexture(const Renderer::TextureDescriptor& desc) override;
	virtual void											   destoryTexture(Renderer::Handle<Renderer::Texture> handle) override;
	[[nodiscard]] virtual Renderer::Handle<Renderer::Buffer>   createBuffer(const Renderer::BufferDescriptor& desc) override;
	virtual void											   destroyBuffer(const Renderer::Handle<Renderer::Buffer>& handle) override;
	[[nodiscard]] virtual Renderer::Handle<Renderer::Sampler>  createSampler(const Renderer::SamplerDescriptor2D& desc) override;
	virtual void											   destroySampler(const Renderer::Handle<Renderer::Sampler>& handle) override;

	GLBuffer*				  GetBuffer(const Renderer::Handle<Renderer::Buffer>& handle);
	Shaders::C_ShaderProgram* GetShader(const Renderer::Handle<Renderer::Shader>& handle);
	GLPipeline*				  GetPipeline(const Renderer::Handle<Renderer::Pipeline>& handle);

private:
	// Renderer::ResourcePool<Renderer::Texture, Textures::C_Texture> m_TexturePool;
	Renderer::ResourcePool<Renderer::Shader, std::shared_ptr<Shaders::C_ShaderProgram>> m_ShaderPool;
	Renderer::ResourcePool<Renderer::Buffer, GLBuffer>									m_BufferPool;
	Renderer::ResourcePool<Renderer::Pipeline, GLPipeline>								m_PipelinePool;
};
} // namespace GLEngine::GLRenderer

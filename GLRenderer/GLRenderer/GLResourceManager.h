#pragma once

#include <GLRenderer/Shaders/ShaderProgram.h>
#include <GLRenderer/Textures/Texture.h>

#include <Renderer/Resources/ResourceManager.h>

namespace GLEngine::GLRenderer {
class GLResourceManager : public Renderer::ResouceManager {
public:
	[[nodiscard]] virtual Renderer::Handle<Renderer::Shader>  createShader(const std::filesystem::path& path) override;
	virtual void											  destoryShader(Renderer::Handle<Renderer::Shader> handle) override;
	[[nodiscard]] virtual Renderer::Handle<Renderer::Texture> createTexture(const Renderer::TextureDescriptor& desc) override;
	virtual void											  destoryTexture(Renderer::Handle<Renderer::Texture> handle) override;

private:
	//Renderer::ResourcePool<Renderer::Shader, Shaders::C_ShaderProgram> m_ShaderPool;
	//Renderer::ResourcePool<Renderer::Texture, Textures::C_Texture> m_TexturePool;
};
} // namespace GLEngine::GLRenderer

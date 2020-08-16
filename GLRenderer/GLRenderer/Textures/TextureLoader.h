#pragma once

namespace GLEngine::GLRenderer::Textures {
class C_Texture;
class TextureLoader
{
public:
	[[nodiscard]] std::shared_ptr<C_Texture> LoadAndInitTexture(const std::filesystem::path& path);
};
}
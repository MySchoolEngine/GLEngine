#pragma once

#include <Renderer/Mesh/Scene.h>

namespace GLEngine::GLRenderer::Textures {
class C_Texture;
class TextureLoader
{
public:
	[[nodiscard]] bool loadTexture(const std::filesystem::path& path, Renderer::MeshData::Texture& t);

	[[nodiscard]] std::shared_ptr<C_Texture> LoadAndInitTexture(const std::filesystem::path& path);

private:
	static bool _isILinitialized;
};
}
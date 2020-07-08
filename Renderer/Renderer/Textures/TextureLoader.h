#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/Mesh/Scene.h>

namespace GLEngine::Renderer::Textures {
class RENDERER_API_EXPORT TextureLoader
{
public:
	[[nodiscard]] bool loadTexture(const std::filesystem::path& path, MeshData::Texture& t);

private:
	static bool _isILinitialized;
};
}
#pragma once

#include <Renderer/Mesh/Scene.h>

namespace GLEngine {
namespace GLRenderer {
namespace Textures {
class TextureLoader
{
public:
	bool loadTexture(const char* path, Renderer::MeshData::Texture& t);

private:
	static bool _isILinitialized;
};
}}}
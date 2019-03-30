#pragma once

#include <GLRenderer/MeshLoading/Scene.h>

namespace GLEngine {
namespace GLRenderer {
namespace Textures {
class TextureLoader
{
public:
	bool loadTexture(const char* path, Mesh::Texture& t);

private:
	static bool _isILinitialized;
};
}}}
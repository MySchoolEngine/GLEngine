#pragma once

#include <Renderer/RendererApi.h>
#include <Renderer/Textures/Storage/TextureStorage.h>

namespace GLEngine::Renderer {
class I_TextureViewStorage;
namespace MeshData {
struct Texture;
}
} // namespace GLEngine::Renderer

namespace GLEngine::Renderer::Textures {
class RENDERER_API_EXPORT TextureLoader {
public:
	[[nodiscard]] bool loadTexture(const std::filesystem::path& path, MeshData::Texture& t);
	// The pointer is owning, caller is responsible for deleting it
	[[nodiscard]] std::unique_ptr<I_TextureViewStorage> loadTexture(const std::filesystem::path& path);
	bool												SaveTexture(const std::filesystem::path& path, I_TextureViewStorage*);

	std::mutex& GetMutex() { return m_Mutex; }

private:
	static std::mutex m_Mutex;
	void		 Init();
	unsigned int ilLoadTexture(const std::filesystem::path& path);
	static bool	 _isILinitialized;
};
} // namespace GLEngine::Renderer::Textures
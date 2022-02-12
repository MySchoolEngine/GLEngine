#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer {
class RENDERER_API_EXPORT C_Viewport {
public:
	C_Viewport(int x, int y, unsigned int width, unsigned int height);
	C_Viewport(int x, int y, const glm::uvec2& resolution);

	[[nodiscard]] glm::uvec2 GetResolution() const;
	[[nodiscard]] glm::ivec2 GetCoordinates() const;


private:
	int			 X;
	int			 Y;
	unsigned int Width;
	unsigned int Height;
};
} // namespace GLEngine::Renderer

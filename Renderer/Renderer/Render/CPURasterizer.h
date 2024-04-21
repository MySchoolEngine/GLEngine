#pragma once

#include <Renderer/Colours.h>
#include <Renderer/RendererApi.h>
#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {

class RENDERER_API_EXPORT C_CPURasterizer {
public:
	C_CPURasterizer(Renderer::C_TextureView& view);
	void DrawLine(const Colours::T_Colour& colour, const glm::ivec2& p1, const glm::ivec2& p2, bool antiAliased = false);
	void DrawCircle(const Colours::T_Colour& colour, const glm::ivec2& p, float radius, bool antiAliased = false);
	void FloodFill(const Colours::T_Colour& colour, const glm::ivec2& p);

private:
	void BresenhamHorizontal(const Colours::T_Colour& colour, glm::ivec2 p1, glm::ivec2 p2);
	void BresenhamVertical(const Colours::T_Colour& colour, glm::ivec2 p1, glm::ivec2 p2);
	void XiaolinWu(const Colours::T_Colour& colour, glm::ivec2 p1, glm::ivec2 p2);

	void					QueueFloodFill(const Colours::T_Colour& colour, const glm::ivec2& p);
	void					ScanLineFloodFill(const Colours::T_Colour& colour, const glm::ivec2& p);
	Renderer::C_TextureView m_view;
};
} // namespace GLEngine::Renderer
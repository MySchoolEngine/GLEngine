#pragma once

#include <Renderer/Colours.h>
#include <Renderer/RendererApi.h>
#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {

class RENDERER_API_EXPORT C_CPURasterizer {
public:
	C_CPURasterizer(Renderer::C_TextureView& view);
	void DrawLine(const Colours::T_Colour& colour, const glm::ivec2& p1, const glm::ivec2& p2, bool antiAliased = false);

private:
	void					BresenhamHorizontal(const Colours::T_Colour& colour, glm::ivec2 p1, glm::ivec2 p2);
	void					BresenhamVertical(const Colours::T_Colour& colour, glm::ivec2 p1, glm::ivec2 p2);
	void					XiaolinWu(const Colours::T_Colour& colour, glm::ivec2 p1, glm::ivec2 p2);
	Renderer::C_TextureView m_view;
};
} // namespace GLEngine::Renderer
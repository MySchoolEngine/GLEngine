#pragma once

#include <Renderer/Colours.h>
#include <Renderer/RendererApi.h>
#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {

class RENDERER_API_EXPORT C_CPURasterizer {
public:
	C_CPURasterizer(const C_TextureView& view);
	void DrawLine(const Colours::T_Colour& colour, const glm::ivec2& p1, const glm::ivec2& p2, bool antiAliased = false);
	struct CircleColorSettings {
		CircleColorSettings(const Colours::T_Colour& c)
			: Colour(c) {} // TODO Just compilation fix
		const Colours::T_Colour&	Colour;
		std::function<float(float)> FalloffFunc = [](float) { return 1.f; };
		bool						bFill		= false;
	};
	void DrawCircle(const CircleColorSettings& colourSettings, const glm::ivec2& p, float radius, bool antiAliased = false);
	void FloodFill(const Colours::T_Colour& colour, const glm::ivec2& p);
	void DrawTriangle(const Colours::T_Colour& colour, const std::array<glm::ivec2, 3>& triangle);
	void DrawAABox(const Colours::T_Colour& colour, const Core::S_Rect& rect);

private:
	void BresenhamHorizontal(const Colours::T_Colour& colour, glm::ivec2 p1, glm::ivec2 p2);
	void BresenhamVertical(const Colours::T_Colour& colour, glm::ivec2 p1, glm::ivec2 p2);
	void XiaolinWu(const Colours::T_Colour& colour, glm::ivec2 p1, glm::ivec2 p2);

	void DrawCircleOutline(const CircleColorSettings& colourSettings, const glm::ivec2& p, float radius, bool antiAliased = false);

	void		  QueueFloodFill(const Colours::T_Colour& colour, const glm::uvec2& p);
	void		  ScanLineFloodFill(const Colours::T_Colour& colour, const glm::uvec2& p);
	C_TextureView m_view;
};
} // namespace GLEngine::Renderer
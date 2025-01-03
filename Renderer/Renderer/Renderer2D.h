#pragma once

#include <Renderer/RendererApi.h>
#include <Renderer/Resources/RenderResourceHandle.h>

namespace GLEngine::Renderer {

// binning first happens by shader, later by geometry
struct RenderCall2D {
	glm::uvec2		  Position; // center of square sized 1x1 units
	glm::vec2		  Size;
	float			  Rotation;
	Handle<Pipeline>  PipelineHandle;
	Colours::T_Colour Colour;
	Handle<Buffer>	  IndirectBuff = {};
	Handle<Buffer>	  HackedArray  = {};
};


class I_RenderInterface2D {
public:
	I_RenderInterface2D()												 = default;
	virtual ~I_RenderInterface2D()										 = default;
	I_RenderInterface2D(const I_RenderInterface2D& other)				 = delete;
	I_RenderInterface2D(I_RenderInterface2D&& other) noexcept			 = delete;
	I_RenderInterface2D& operator=(const I_RenderInterface2D& other)	 = delete;
	I_RenderInterface2D& operator=(I_RenderInterface2D&& other) noexcept = delete;

	// I need to draw, draw instanced etc.
	virtual void Render(const RenderCall2D& call) = 0;
};


class RENDERER_API_EXPORT C_Renderer2D {
public:
	void Draw(RenderCall2D&& call); // 1]
	void Clear();
	void Commit(I_RenderInterface2D& interface); // resource manager needed
private:
	std::vector<RenderCall2D> m_DrawCalls;
};
} // namespace GLEngine::Renderer
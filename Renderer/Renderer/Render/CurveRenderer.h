#pragma once

#include <Renderer/Colours.h>
#include <Renderer/DebugDraw.h>
#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer {
template <class pointType> class I_CurveFunction;

class RENDERER_API_EXPORT C_3DCurveRenderer {
public:
	C_3DCurveRenderer(I_DebugDraw& DD);

	// Single colour draw
	void Draw(const I_CurveFunction<glm::vec3>& curve, const Colours::T_Colour& color, std::size_t divisions = 50);
	// Two colour draw
	void Draw(const I_CurveFunction<glm::vec3>& curve, const Colours::T_Colour& colorT0, const Colours::T_Colour& colorT1, std::size_t divisions = 50);

private:
	I_DebugDraw& dd;
};
} // namespace GLEngine::Renderer

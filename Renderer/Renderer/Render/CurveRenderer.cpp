#include <RendererStdafx.h>

#include <Renderer/Mesh/Curve.h>
#include <Renderer/Render/CurveRenderer.h>

namespace GLEngine::Renderer {

//=================================================================================
C_3DCurveRenderer::C_3DCurveRenderer(I_DebugDraw& DD)
	: dd(DD)
{
}

//=================================================================================
void C_3DCurveRenderer::Draw(const I_CurveFunction<glm::vec3>& curve, const Colours::T_Colour& color, std::size_t divisions /*= 50*/)
{
	const auto part = 1.0 / divisions;
	for (int i = 0; i < divisions; ++i)
	{
		dd.DrawLine(curve.GetPointInTime(part * i), curve.GetPointInTime(part * (i + 1)), color);
	}
}

//=================================================================================
void C_3DCurveRenderer::Draw(const I_CurveFunction<glm::vec3>& curve, const Colours::T_Colour& colorT0, const Colours::T_Colour& colorT1, std::size_t divisions /*= 50*/)
{
	const auto part = 1.0 / divisions;
	for (int i = 0; i < divisions; ++i)
	{
		dd.DrawLine(curve.GetPointInTime(part * i), curve.GetPointInTime(part * (i + 1)), glm::mix(colorT0, colorT1, part * i), glm::mix(colorT0, colorT1, part * (i + 1)));
	}
}

} // namespace GLEngine::Renderer
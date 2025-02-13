#include <RendererStdafx.h>

#include <Renderer/Render/CPURasterizer.h>
#include <Renderer/Render/PhysicsRenderer.h>

#include <Core/Math/Rect.h>

namespace GLEngine::Renderer {
void C_PhysicsRenderer::Render(C_TextureView& view, const Physics::Primitives::Plane2D& plane, const Colours::T_Colour& colour)
{
	C_CPURasterizer			  rasterizer(view);
	const auto				  rect = view.GetRect();
	std::array<glm::ivec2, 2> Intersects;
	if (rect.IntersectionPlane(plane.Position, plane.GetDirection(), Intersects))
	{
		rasterizer.DrawLine(colour, Intersects[0], Intersects[1], true);
	}
}

void C_PhysicsRenderer::Render(C_TextureView& view, const Core::S_Rect& box2D, const Colours::T_Colour& colour)
{
	C_CPURasterizer rasterizer(view);
	rasterizer.DrawLine(colour, box2D.BottomLeft(), box2D.TopLeft(), false);
	rasterizer.DrawLine(colour, box2D.BottomLeft(), box2D.BottomRight(), false);
	rasterizer.DrawLine(colour, box2D.BottomRight(), box2D.TopRight(), false);
	rasterizer.DrawLine(colour, box2D.TopLeft(), box2D.TopRight(), false);
}
} // namespace GLEngine::Renderer
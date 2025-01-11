#include <RendererStdafx.h>

#include <Renderer/Render/CPURasterizer.h>
#include <Renderer/Render/PhysicsRenderer.h>

#include <glm/gtx/intersect.hpp>

namespace GLEngine::Renderer {
void C_PhysicsRenderer::Render(C_TextureView& view, const Physics::Primitives::Plane2D& plane, const Colours::T_Colour& colour)
{
	C_CPURasterizer rasterizer(view);
	const auto				  rect		 = view.GetRect();
	std::array<glm::ivec2, 2> Intersects;
	if (rect.IntersectionPlane(plane.Position, plane.GetDirection(), Intersects))
	{
		rasterizer.DrawLine(colour, Intersects[0], Intersects[1], true);
	}
}
} // namespace GLEngine::Renderer
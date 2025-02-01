#pragma once

#include <Renderer/RendererApi.h>

#include <Physics/2D/LineSegment.h>
#include <Physics/2D/Plane2D.h>

namespace GLEngine::Renderer {
class C_TextureView;
// bottom of the world is y = 0
// smaller x is left
class RENDERER_API_EXPORT C_PhysicsRenderer {
public:
	static void Render(C_TextureView& view, const Physics::Primitives::Plane2D& plane, const Colours::T_Colour& colour);
	static void Render(C_TextureView& view, const Core::S_Rect& box2D, const Colours::T_Colour& colour);
	static void Render(C_TextureView& view, const Physics::Primitives::S_LineSegment& plane, const Colours::T_Colour& colour);
};
} // namespace GLEngine::Renderer
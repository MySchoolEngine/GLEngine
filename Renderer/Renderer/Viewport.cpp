#include <RendererStdAfx.h>

#include <Renderer/Viewport.h>

namespace GLEngine::Renderer {

//=================================================================================
C_Viewport::C_Viewport(int x, int y, unsigned int width, unsigned int height)
	: X(x)
	, Y(y)
	, Width(width)
	, Height(height)
{
}

//=================================================================================
C_Viewport::C_Viewport(int x, int y, const glm::uvec2& resolution)
	: X(x)
	, Y(y)
	, Width(resolution.x)
	, Height(resolution.y)
{
}

//=================================================================================
glm::uvec2 C_Viewport::GetResolution() const
{
	return {Width, Height};
}

//=================================================================================
glm::ivec2 C_Viewport::GetCoordinates() const
{
	return {X, Y};
}

} // namespace GLEngine::Renderer

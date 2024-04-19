#include <RendererStdafx.h>

#include <Renderer/Render/CPURasterizer.h>


namespace GLEngine::Renderer {

//=================================================================================
C_CPURasterizer::C_CPURasterizer(Renderer::C_TextureView& view)
	: m_view(view)
{
}

//=================================================================================
void C_CPURasterizer::DrawLine(const Colours::T_Colour& colour, const glm::ivec2& p1, const glm::ivec2& p2, bool antiAliased)
{
	if (antiAliased)
	{
		XiaolinWu(colour, p1, p2);
	}
	else
	{
		if (std::abs(p2.y - p1.y) < std::abs(p2.x - p1.x))
		{
			BresenhamHorizontal(colour, p1, p2);
		}
		else
		{
			BresenhamVertical(colour, p1, p2);
		}
	}
}

//=================================================================================
void C_CPURasterizer::BresenhamHorizontal(const Colours::T_Colour& colour, glm::ivec2 p1, glm::ivec2 p2)
{
	// Bresenham's algorithm
	if (p1.x > p2.x)
	{
		std::swap(p1, p2);
	}

	const int	 dx		= p2.x - p1.x;
	int			 dy		= p2.y - p1.y;
	unsigned int y		= p1.y;
	int			 pred	= 2 * dy - dx;
	int			 ydelta = 1;
	if (dy < 0)
	{
		dy	   = -dy;
		ydelta = -ydelta;
	}

	for (int x = p1.x; x <= p2.x; ++x)
	{
		m_view.Set(glm::ivec2(x, y), glm::vec3(colour));
		if (pred > 0)
		{
			y += ydelta;
			pred = pred + 2 * dy - 2 * dx;
		}
		else
		{
			pred = pred + 2 * dy;
		}
	}
}

//=================================================================================
void C_CPURasterizer::BresenhamVertical(const Colours::T_Colour& colour, glm::ivec2 p1, glm::ivec2 p2)
{
	// Bresenham's algorithm
	if (p1.y > p2.y)
	{
		std::swap(p1, p2);
	}

	int			 dx		= p2.x - p1.x;
	const int	 dy		= p2.y - p1.y;
	unsigned int x		= p1.x;
	int			 pred	= 2 * dx - dy;
	int			 xdelta = 1;
	if (dx < 0)
	{
		dx	   = -dx;
		xdelta = -xdelta;
	}

	for (int y = p1.y; y <= p2.y; ++y)
	{
		m_view.Set(glm::ivec2(x, y), glm::vec3(colour));
		if (pred > 0)
		{
			x += xdelta;
			pred = pred + 2 * dx - 2 * dy;
		}
		else
		{
			pred = pred + 2 * dx;
		}
	}
}

//=================================================================================
void C_CPURasterizer::XiaolinWu(const Colours::T_Colour& colour, glm::ivec2 p1, glm::ivec2 p2)
{
	bool vertical = std::abs(p2.y - p1.y) > std::abs(p2.x - p1.x);
	if (vertical) {
		std::swap(p1.x, p1.y);
		std::swap(p2.x, p2.y);
	}
	if (p1.x > p2.x)
	{
		std::swap(p1, p2);
	}

	int		  dx	   = p2.x - p1.x;
	const int dy	   = p2.y - p1.y;
	float	  gradient = 1.0f;
	if (dx != 0)
	{
		gradient = static_cast<float>(dy) / dx;
	}
	// float yend = y0 + gradient * (xend - p1.x);
	float yIntersect = p1.y + gradient;

	const auto fract	  = [](float x) { return x - std::floor(x); };
	// endpoints
	

	if (vertical)
	{
		for (int x = p1.x + 1; x < p2.x; ++x)
		{
			// needs alpha blending
			const auto fraction = fract(yIntersect);
			m_view.DrawPixel(glm::ivec2(std::floor(yIntersect), x), glm::vec4(colour, 1.f - fraction));
			m_view.DrawPixel(glm::ivec2(std::floor(yIntersect) + 1, x), glm::vec4(colour, fraction));
			yIntersect = yIntersect + gradient;
		}
	}
	else
	{
		for (int x = p1.x + 1; x < p2.x; ++x)
		{
			// needs alpha blending
			const auto fraction = fract(yIntersect);
			m_view.DrawPixel(glm::ivec2(x, std::floor(yIntersect)), glm::vec4(colour, 1.f - fraction));
			m_view.DrawPixel(glm::ivec2(x, std::floor(yIntersect) + 1), glm::vec4(colour, fraction));
			yIntersect = yIntersect + gradient;
		}
	}
}

} // namespace GLEngine::Renderer
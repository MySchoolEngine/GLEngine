#include <RendererStdafx.h>

#include <Renderer/Render/CPURasterizer.h>

#include <queue>

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
void C_CPURasterizer::DrawCircle(const Colours::T_Colour& colour, const glm::ivec2& p, float radius, bool antiAliased)
{
	if (antiAliased)
	{
		float t1 = radius / 16;
		float t2 = 0.f;
		int	  y	 = 0;
		int	  x	 = radius;
		float xIntersect = 0.f;
		while (x >= y)
		{
			m_view.Set(p + glm::ivec2{x, y}, glm::vec3{colour});
			m_view.Set(p + glm::ivec2{x, -y}, glm::vec3{colour});
			m_view.Set(p + glm::ivec2{-x, -y}, glm::vec3{colour});
			m_view.Set(p + glm::ivec2{-x, y}, glm::vec3{colour});
			m_view.Set(p + glm::ivec2{y, x}, glm::vec3{colour});
			m_view.Set(p + glm::ivec2{y, -x}, glm::vec3{colour});
			m_view.Set(p + glm::ivec2{-y, -x}, glm::vec3{colour});
			m_view.Set(p + glm::ivec2{-y, x}, glm::vec3{colour});
			++y;
			t1 = t1 + y;
			t2 = t1 - x;
			if (t2 >= 0)
			{
				t1 = t2;
				--x;
			}
		}
	}
	else
	{
		float t1 = radius / 16;
		float t2 = 0.f;
		int	  y	 = 0;
		int	  x	 = radius;
		while (x >= y)
		{
			m_view.Set(p + glm::ivec2{x, y}, glm::vec3{colour});
			m_view.Set(p + glm::ivec2{x, -y}, glm::vec3{colour});
			m_view.Set(p + glm::ivec2{-x, -y}, glm::vec3{colour});
			m_view.Set(p + glm::ivec2{-x, y}, glm::vec3{colour});
			m_view.Set(p + glm::ivec2{y, x}, glm::vec3{colour});
			m_view.Set(p + glm::ivec2{y, -x}, glm::vec3{colour});
			m_view.Set(p + glm::ivec2{-y, -x}, glm::vec3{colour});
			m_view.Set(p + glm::ivec2{-y, x}, glm::vec3{colour});
			++y;
			t1 = t1 + y;
			t2 = t1 - x;
			if (t2 >= 0)
			{
				t1 = t2;
				--x;
			}
		}
	}
}

//=================================================================================
void C_CPURasterizer::FloodFill(const Colours::T_Colour& colour, const glm::ivec2& p)
{
	ScanLineFloodFill(colour, p);
}

//=================================================================================
void C_CPURasterizer::QueueFloodFill(const Colours::T_Colour& colour, const glm::uvec2& p)
{
	const glm::vec3		   clearColor = m_view.Get<glm::vec3>(p);
	std::queue<glm::ivec2> open;
	open.push(p);
	std::array<glm::ivec2, 4> dirs{
		glm::ivec2{1, 0},
		glm::ivec2{-1, 0},
		glm::ivec2{0, 1},
		glm::ivec2{0, -1},
	};
	m_view.Set(p, glm::vec3{colour});
	while (open.empty() == false)
	{
		const auto current = open.front();
		open.pop();
		for (const auto& dir : dirs)
		{
			const auto		testCoord	 = current + dir;
			if (!m_view.GetRect().Contains(testCoord))
				continue;
			const glm::vec3 testedColour = m_view.Get<glm::vec3>(glm::uvec2{testCoord});
			if (testedColour == clearColor)
			{
				m_view.Set(current + dir, glm::vec3{colour});
				open.push(current + dir);
			}
		}
	}
}

//=================================================================================
void C_CPURasterizer::ScanLineFloodFill(const Colours::T_Colour& colour, const glm::uvec2& p)
{
	const glm::vec3		   clearColor = m_view.Get<glm::vec3>(p);
	std::queue<glm::uvec2> open;
	const auto			   scanLine = [&](int min, int max, int line) {
		bool spanAdded = false;
		for (int i = min; i < max; ++i)
		{
			if (m_view.Get<glm::vec3>(glm::uvec2{i, line}) != clearColor)
			{
				spanAdded = false;
			}
			else if (!spanAdded)
			{
				open.push(glm::ivec2{i, line});
				spanAdded = true;
			}
		}
	};
	open.push(p);
	m_view.Set(p, glm::vec3{colour});
	while (open.empty() == false)
	{
		glm::ivec2				current = open.front();
		const static glm::ivec2 leftStep{1, 0};
		open.pop();
		glm::ivec2 leftCurrent = current;
		while (m_view.Get<glm::vec3>(glm::uvec2{leftCurrent - leftStep}) == clearColor && leftCurrent.x > 0)
		{
			leftCurrent -= leftStep;
		}
		while (m_view.Get<glm::vec3>(glm::uvec2{leftCurrent - leftStep}) == clearColor && m_view.GetRect().Contains(current))
		{
			current += leftStep;
		}
		m_view.FillLineSpan(colour, current.y, leftCurrent.x, current.x);
		scanLine(leftCurrent.x, current.x, current.y - 1);
		scanLine(leftCurrent.x, current.x, current.y + 1);
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
	if (vertical)
	{
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

	const auto fract = [](float x) { return x - std::floor(x); };
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
#include <EditorStdafx.h>

#include <Editor/Editors/Image/Tools/PerlinNoiseGenerator.h>

#include <GUI/ReflectionGUI.h>

#include <numbers>
#include <rttr/registration>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Editor;
	using namespace Utils::Reflection;

}
// clang-format on

namespace GLEngine::Editor {
//=================================================================================
C_PerlinNoise::C_PerlinNoise(Renderer::C_TextureView view)
	: C_ImageEditorTool(view)
{
}

//=================================================================================
void C_PerlinNoise::Apply()
{
	for (unsigned int x = 0; x < m_View.GetDimensions().x; ++x)
	{
		for (unsigned int y = 0; y < m_View.GetDimensions().y; ++y)
		{
			float val = m_Frequency * PNoise({x,y}, 6);

			m_View.Set(glm::ivec2{x, y}, glm::vec3{val, val, val});
		}
	}
}

//=================================================================================
float C_PerlinNoise::PNoise(const glm::uvec2& coord, int res)
{
	float persistance = .5;
	float n			  = 0.;
	float normK		  = 0.;
	float f			  = 4.;
	float amp		  = 1.;
	for (int i = 0; i < res; i++)
	{
		n += amp * Noise(coord, f);
		f *= 2.;
		normK += amp;
		amp *= persistance;
	}
	float nf = n / normK;
	return nf * nf * nf * nf;
}

//=================================================================================
float C_PerlinNoise::Noise(const glm::uvec2& coord, float freq)
{
	const auto rand = [](const glm::vec2& c) { return glm::fract(glm::sin(glm::dot(c, glm::vec2(12.9898f, 78.233f))) * 43758.5453f); };
	float unit = m_View.GetDimensions().x / freq;
	glm::vec2  ij	= glm::floor(glm::vec2{coord} / unit);
	glm::vec2  xy	= glm::mod(glm::vec2{coord}, unit) / unit;
	// xy = 3.*xy*xy-2.*xy*xy*xy;
	xy		 = .5f * (1.f - glm::cos(std::numbers::pi_v<float> * xy));
	float a	 = rand((ij + glm::vec2(0., 0.)));
	float b	 = rand((ij + glm::vec2(1., 0.)));
	float c	 = rand((ij + glm::vec2(0., 1.)));
	float d	 = rand((ij + glm::vec2(1., 1.)));
	float x1 = glm::mix(a, b, xy.x);
	float x2 = glm::mix(c, d, xy.x);
	return glm::mix(x1, x2, xy.y);
}

} // namespace GLEngine::Editor
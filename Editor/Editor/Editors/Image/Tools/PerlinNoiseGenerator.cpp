#include <EditorStdafx.h>

#include <Editor/Editors/Image/Tools/PerlinNoiseGenerator.h>

#include <GUI/ReflectionGUI.h>

#include <Utils/Serialization/SerializationUtils.h>

#include <numbers>
#include <rttr/registration>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Editor;
	using namespace Utils::Reflection;
	
	rttr::registration::class_<C_PerlinNoise>("C_PerlinNoise")
		.property("Frequency", &C_PerlinNoise::m_Frequency)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::SliderInt>(),
			RegisterMetamember<UI::SliderInt::Name>("Frequency:"),
			RegisterMetamember<UI::SliderInt::Min>(1),
			RegisterMetamember<UI::SliderInt::Max>(100),
			REGISTER_DEFAULT_VALUE(1))
		.property("Persistence", &C_PerlinNoise::m_Persistence)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Slider>(),
			RegisterMetamember<UI::Slider::Name>("Persistence:"),
			RegisterMetamember<UI::Slider::Min>(1e-4f),
			RegisterMetamember<UI::Slider::Max>(1.f),
			REGISTER_DEFAULT_VALUE(1));
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
			const float val = PNoise({x, y}, 6) * m_Frequency;

			m_View.Set(glm::ivec2{x, y}, glm::vec4{val, val, val, 1.f});
		}
	}
}

//=================================================================================
float C_PerlinNoise::PNoise(const glm::uvec2& coord, int res)
{
	float n		= 0.;
	float normK = 0.;
	float f		= 4.;
	float amp	= 1.;
	for (int i = 0; i < res; i++)
	{
		n += amp * Noise(coord, f);
		f *= 2.f;
		normK += amp;
		amp *= m_Persistence;
	}
	const float nf = n / normK;
	return nf * nf * nf * nf;
}

//=================================================================================
float C_PerlinNoise::Noise(const glm::uvec2& coord, float freq)
{
	const auto rand = [](const glm::vec2& c) { return glm::fract(glm::sin(glm::dot(c, glm::vec2(12.9898f, 78.233f))) * 43758.5453f); };
	float	   unit = m_View.GetDimensions().x / freq;
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
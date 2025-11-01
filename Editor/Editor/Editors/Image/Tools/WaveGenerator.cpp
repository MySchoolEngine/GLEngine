#include <EditorStdafx.h>

#include <Editor/Editors/Image/Tools/WaveGenerator.h>

#include <GUI/ReflectionGUI.h>

#include <rttr/registration>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Editor;
	using namespace Utils::Reflection;

	rttr::registration::class_<C_WaveGenerator>("C_WaveGenerator")
		.property("Height", &C_WaveGenerator::m_Center)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::iVec2>(),
			RegisterMetamember<UI::iVec2::Name>("Wave center:")
			)
		.property("WaveSize", &C_WaveGenerator::m_WaveSize)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Slider>(),
			RegisterMetamember<UI::Slider::Name>("Wave size:"),
			RegisterMetamember<UI::Slider::Min>(1.f),
			RegisterMetamember<UI::Slider::Max>(100.f)
			)
	;
}
// clang-format on

namespace GLEngine::Editor {
//=================================================================================
C_WaveGenerator::C_WaveGenerator(Renderer::C_TextureView view)
	: C_ImageEditorTool(view)
	, m_WaveSize(10)
{
	m_Center = {view.GetRect().GetWidth()/2, view.GetRect().GetHeight()/2};
}

//=================================================================================
void C_WaveGenerator::Apply()
{
	const auto rect = m_View.GetRect();

	for (unsigned int x = rect.Left(); x < rect.GetWidth(); ++x)
	{
		for (unsigned int y = rect.Top(); y < rect.GetHeight(); ++y)
		{
			const glm::uvec2 point{x, y};
			const auto		 distance = glm::distance(glm::vec2{point}, glm::vec2{m_Center});
			const auto		 value	  = std::sin(distance/ m_WaveSize);
			m_View.Set(point, glm::vec4{value, value, value, 1.f});
		}
	}

}

} // namespace GLEngine::Editor
#include <EditorStdafx.h>

#include <Editor/Editors/Image/Tools/BrickGenerator.h>

#include <GUI/ReflectionGUI.h>

#include <rttr/registration>

RTTR_REGISTRATION
{
	using namespace GLEngine::Editor;
	using namespace Utils::Reflection;

	rttr::registration::class_<C_BrickGenerator>("C_BrickGenerator")
		.property("Height", &C_BrickGenerator::m_RowHeight)(
			rttr::policy::prop::bind_as_ptr,
			RegisterMetaclass<MetaGUI::SliderInt>(),
			RegisterMetamember<UI::SliderInt::Name>("Row height:"),
			RegisterMetamember<UI::SliderInt::Min>(1),
			RegisterMetamember<UI::SliderInt::Max>(100)
			)
		.property("BrickWidth", &C_BrickGenerator::m_BrickWidth)(
			rttr::policy::prop::bind_as_ptr,
			RegisterMetaclass<MetaGUI::SliderInt>(),
			RegisterMetamember<UI::SliderInt::Name>("Row width:"),
			RegisterMetamember<UI::SliderInt::Min>(1),
			RegisterMetamember<UI::SliderInt::Max>(100)
			)
		.property("BrickOffset", &C_BrickGenerator::m_BrickOffset)(
			rttr::policy::prop::bind_as_ptr,
			RegisterMetaclass<MetaGUI::SliderInt>(),
			RegisterMetamember<UI::SliderInt::Name>("Brick offset:"),
			RegisterMetamember<UI::SliderInt::Min>(1),
			RegisterMetamember<UI::SliderInt::Max>(100)
			)
		.property("MortarThickness", &C_BrickGenerator::m_MortarThickness)(
			rttr::policy::prop::bind_as_ptr,
			RegisterMetaclass<MetaGUI::SliderInt>(),
			RegisterMetamember<UI::SliderInt::Name>("Mortar thickness:"),
			RegisterMetamember<UI::SliderInt::Min>(1),
			RegisterMetamember<UI::SliderInt::Max>(100)
			)
		.property("BrickColour", &C_BrickGenerator::m_BrickColour)(
			rttr::policy::prop::bind_as_ptr,
			RegisterMetaclass<MetaGUI::Colour>(),
			RegisterMetamember<UI::Colour::Name>("Brick colour:")
			)
		.property("MortarColour", &C_BrickGenerator::m_MortarColour)(
			rttr::policy::prop::bind_as_ptr,
			RegisterMetaclass<MetaGUI::Colour>(),
			RegisterMetamember<UI::Colour::Name>("Mortar colour:")
			);
}



namespace GLEngine::Editor {
//=================================================================================
C_BrickGenerator::C_BrickGenerator(Renderer::C_TextureView view)
	: C_ImageEditorTool(view)
	, m_RowHeight(10)
	, m_BrickWidth(30)
	, m_MortarThickness(2)
	, m_BrickOffset(20)
	, m_BrickColour(Colours::T_Colour{227.f / 255.f, 3.f / 255.f, 12.f / 255.f})
	, m_MortarColour(Colours::T_Colour{93.f / 255.f, 93.f / 255.f, 93.f / 255.f})
{
}

void C_BrickGenerator::Apply()
{
	m_View.ClearColor({m_MortarColour, 1.f});
	unsigned int mortarTop	 = 0;
	int			 totalOffset = 0;
	int			 row		 = 0;
	while (mortarTop < m_View.GetDimensions().y)
	{
		int mortarLeft = totalOffset;
		while (mortarLeft < static_cast<int>(m_View.GetDimensions().x))
		{
			for (int i = 0; i < m_RowHeight; ++i)
			{
				m_View.FillLineSpan(m_BrickColour, mortarTop + m_MortarThickness + i, std::max(0, mortarLeft + m_MortarThickness), mortarLeft + m_MortarThickness + m_BrickWidth - 1);
			}
			mortarLeft += m_MortarThickness + m_BrickWidth;
		}
		mortarTop += m_MortarThickness + m_RowHeight;
		totalOffset -= m_BrickOffset;
		if (totalOffset <= -(m_BrickWidth + m_MortarThickness))
			totalOffset += (m_BrickWidth + m_MortarThickness);

		row++;
	}
}

} // namespace GLEngine::Editor
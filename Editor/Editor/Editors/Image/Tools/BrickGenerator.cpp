#include <EditorStdafx.h>

#include <Editor/Editors/Image/Tools/BrickGenerator.h>

#include <GUI/ReflectionGUI.h>

#include <random>
#include <rttr/registration>

// clang-format off
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
			RegisterMetamember<UI::SliderInt::Max>(100))
		.property("BrickWidth", &C_BrickGenerator::m_BrickWidth)(
			rttr::policy::prop::bind_as_ptr,
			RegisterMetaclass<MetaGUI::SliderInt>(),
			RegisterMetamember<UI::SliderInt::Name>("Row width:"),
			RegisterMetamember<UI::SliderInt::Min>(1),
			RegisterMetamember<UI::SliderInt::Max>(100))
		.property("BrickOffset", &C_BrickGenerator::m_BrickOffset)(
			rttr::policy::prop::bind_as_ptr,
			RegisterMetaclass<MetaGUI::SliderInt>(),
			RegisterMetamember<UI::SliderInt::Name>("Brick offset:"),
			RegisterMetamember<UI::SliderInt::Min>(1),
			RegisterMetamember<UI::SliderInt::Max>(100))
#pragma region Randomness
		.property("RowHeightDeviation", &C_BrickGenerator::m_RowHeightDeviation)(
			rttr::policy::prop::bind_as_ptr,
			RegisterMetaclass<MetaGUI::SliderInt>(),
			RegisterMetamember<MetaGUIInfo::CollapsableGroup>("Randomness"),
			RegisterMetamember<UI::SliderInt::Name>("Row height deviation:"),
			RegisterMetamember<UI::SliderInt::Min>(1),
			RegisterMetamember<UI::SliderInt::Max>(100))
		.property("BrickWidthDeviation", &C_BrickGenerator::m_BrickWidthDeviation)(
			rttr::policy::prop::bind_as_ptr,
			RegisterMetaclass<MetaGUI::SliderInt>(),
			RegisterMetamember<MetaGUIInfo::CollapsableGroup>("Randomness"),
			RegisterMetamember<UI::SliderInt::Name>("Brick width deviation:"),
			RegisterMetamember<UI::SliderInt::Min>(1),
			RegisterMetamember<UI::SliderInt::Max>(100))
		.property("MortarWidthDeviation", &C_BrickGenerator::m_MortarWidthDeviation)(
			rttr::policy::prop::bind_as_ptr,
			RegisterMetaclass<MetaGUI::SliderInt>(),
			RegisterMetamember<MetaGUIInfo::CollapsableGroup>("Randomness"),
			RegisterMetamember<UI::SliderInt::Name>("Mortar width deviation:"),
			RegisterMetamember<UI::SliderInt::Min>(1),
			RegisterMetamember<UI::SliderInt::Max>(100))
		.property("MortarHeightDeviation", &C_BrickGenerator::m_MortarHeightDeviation)(
			rttr::policy::prop::bind_as_ptr,
			RegisterMetaclass<MetaGUI::SliderInt>(),
			RegisterMetamember<MetaGUIInfo::CollapsableGroup>("Randomness"),
			RegisterMetamember<UI::SliderInt::Name>("Mortar height deviation:"),
			RegisterMetamember<UI::SliderInt::Min>(1),
			RegisterMetamember<UI::SliderInt::Max>(100))
#pragma endregion
		.property("MortarThickness", &C_BrickGenerator::m_MortarThickness)(
			rttr::policy::prop::bind_as_ptr,
			RegisterMetaclass<MetaGUI::SliderInt>(),
			RegisterMetamember<UI::SliderInt::Name>("Mortar thickness:"),
			RegisterMetamember<UI::SliderInt::Min>(1),
			RegisterMetamember<UI::SliderInt::Max>(100))
		.property("BrickColour", &C_BrickGenerator::m_BrickColour)(
			rttr::policy::prop::bind_as_ptr,
			RegisterMetaclass<MetaGUI::Colour>(),
			RegisterMetamember<UI::Colour::Name>("Brick colour:"))
		.property("MortarColour", &C_BrickGenerator::m_MortarColour)(
			rttr::policy::prop::bind_as_ptr,
			RegisterMetaclass<MetaGUI::Colour>(),
			RegisterMetamember<UI::Colour::Name>("Mortar colour:")
			);
}
// clang-format on

namespace GLEngine::Editor {
//=================================================================================
C_BrickGenerator::C_BrickGenerator(Renderer::C_TextureView view)
	: C_ImageEditorTool(view)
	, m_RowHeight(10)
	, m_BrickWidth(30)
	, m_MortarThickness(2)
	, m_BrickOffset(20)
	, m_RowHeightDeviation(0)
	, m_BrickWidthDeviation(0)
	, m_MortarWidthDeviation(0)
	, m_MortarHeightDeviation(0)
	, m_BrickColour(Colours::T_Colour{227.f / 255.f, 3.f / 255.f, 12.f / 255.f})
	, m_MortarColour(Colours::T_Colour{93.f / 255.f, 93.f / 255.f, 93.f / 255.f})
{
}

//=================================================================================
void C_BrickGenerator::Apply()
{
	std::random_device				rd;		   // a seed source for the random number engine
	std::mt19937					gen(rd()); // mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> rowHeightGen(-m_RowHeightDeviation / 2, m_RowHeightDeviation / 2);
	std::uniform_int_distribution<> brickWidthGen(-m_BrickWidthDeviation / 2, m_BrickWidthDeviation / 2);
	std::uniform_int_distribution<> mortarWidthGen(-m_MortarWidthDeviation / 2, m_MortarWidthDeviation / 2);
	std::uniform_int_distribution<> mortarHeightGen(-m_MortarHeightDeviation / 2, m_MortarHeightDeviation / 2);

	m_View.ClearColor({m_MortarColour, 1.f});
	unsigned int mortarTop	 = 0;
	int			 totalOffset = 0;
	int			 row		 = 0;
	while (mortarTop < m_View.GetDimensions().y)
	{
		int mortarLeft = totalOffset;
		int rowHeight  = m_RowHeight + rowHeightGen(gen);
		while (mortarLeft < static_cast<int>(m_View.GetDimensions().x))
		{
			int brickWidth			= m_BrickWidth + brickWidthGen(gen);
			int leftMortarThickness = m_MortarThickness + mortarWidthGen(gen);
			int brickEnd			= mortarLeft + leftMortarThickness + brickWidth - 1;
			if (brickEnd >= 0)
			{
				for (int i = 0; i < rowHeight; ++i)
				{
					m_View.FillLineSpan(m_BrickColour, mortarTop + m_MortarThickness + i, std::max(0, mortarLeft + leftMortarThickness),
										mortarLeft + leftMortarThickness + brickWidth - 1);
				}
			}
			mortarLeft += leftMortarThickness + brickWidth;
		}
		mortarTop += m_MortarThickness + rowHeight + mortarHeightGen(gen);
		totalOffset -= m_BrickOffset;
		if (totalOffset <= -2 * (m_BrickWidth + m_MortarThickness))
			totalOffset += (m_BrickWidth + m_MortarThickness);

		row++;
	}
}

} // namespace GLEngine::Editor
#include <CoreStdafx.h>

#include <Core/Math/Rect.h>

#include <Utils/Reflection/Metadata.h>

#include <rttr/registration>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Core;
	using namespace Utils::Reflection;

	rttr::registration::class_<S_Rect>("S_Rect")
		.constructor()
		.property("x", &S_Rect::x)
		(
			rttr::policy::prop::as_reference_wrapper, 
			RegisterMetaclass<MetaGUI::SliderInt>(),
			RegisterMetamember<UI::Slider::Name>("x:"),
			RegisterMetamember<UI::Slider::Min>(0),
			RegisterMetamember<UI::Slider::Max>(4096)
		)
		.property("y", &S_Rect::y)
		(
			rttr::policy::prop::as_reference_wrapper, 
			RegisterMetaclass<MetaGUI::SliderInt>(),
			RegisterMetamember<UI::Slider::Name>("y:"),
			RegisterMetamember<UI::Slider::Min>(0),
			RegisterMetamember<UI::Slider::Max>(4096)
		)
		.property("width", &S_Rect::width)
		(
			rttr::policy::prop::as_reference_wrapper, 
			RegisterMetaclass<MetaGUI::SliderInt>(),
			RegisterMetamember<UI::Slider::Name>("width:"),
			RegisterMetamember<UI::Slider::Min>(0),
			RegisterMetamember<UI::Slider::Max>(4096)
		)
		.property("height", &S_Rect::height)
		(
			rttr::policy::prop::as_reference_wrapper, 
			RegisterMetaclass<MetaGUI::SliderInt>(),
			RegisterMetamember<UI::Slider::Name>("height:"),
			RegisterMetamember<UI::Slider::Min>(0),
			RegisterMetamember<UI::Slider::Max>(4096)
		)
		;
}
// clang-format on
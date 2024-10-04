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
		//(
		//	rttr::policy::prop::as_reference_wrapper,
		//	RegisterMetaclass<MetaGUI::IntSlider>(),
		//	RegisterMetamember<UI::Slider::Name>("x:"),
		//	RegisterMetamember<UI::Slider::Min>(0.f),
		//	RegisterMetamember<UI::Slider::Max>(1.0f)
		//)
		;
}
// clang-format on
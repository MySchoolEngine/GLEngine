#include <RendererStdafx.h>

#include <Renderer/Components/Lights/LightComponents.h>

#include <Entity/ComponentRegistry.h>

#include <Utils/Reflection/Metadata.h>
#include <Utils/Serialization/SerializationUtils.h>

#include <rttr/registration>

// clang-format off
RTTR_REGISTRATION
{
	using namespace ::Utils::Reflection;
	using namespace GLEngine::Renderer;

	rttr::registration::class_<S_PointLight>("PointLight")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("Intensity", &S_PointLight::intensity)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Intensity:"),
				RegisterMetamember<UI::Slider::Min>(0.f),
				RegisterMetamember<UI::Slider::Max>(100.0f)
			)
		.property("Color", &S_PointLight::color)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Colour>(),
				RegisterMetamember<UI::Colour::Name>("Colour:")
			);

	rttr::registration::class_<S_AreaLight>("AreaLight")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("Width", &S_AreaLight::width)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Width:"),
				RegisterMetamember<UI::Slider::Min>(.1f),
				RegisterMetamember<UI::Slider::Max>(10.0f)
			)
		.property("Height", &S_AreaLight::height)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Height:"),
				RegisterMetamember<UI::Slider::Min>(.1f),
				RegisterMetamember<UI::Slider::Max>(10.0f)
			)
		.property("DiffuseColour", &S_AreaLight::diffuseColor)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Colour>(),
				RegisterMetamember<UI::Colour::Name>("Diffuse colour:")
			)
		.property("SpecColour", &S_AreaLight::specularColor)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Colour>(),
				RegisterMetamember<UI::Colour::Name>("Spec colour:")
			);

	rttr::registration::class_<S_SunLight>("SunLight")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("SunX", &S_SunLight::sunX)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Sun X:"),
				RegisterMetamember<UI::Slider::Min>(-1.0f),
				RegisterMetamember<UI::Slider::Max>(1.0f)
			)
		.property("SunY", &S_SunLight::sunY)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Sun Y:"),
				RegisterMetamember<UI::Slider::Min>(-1.0f),
				RegisterMetamember<UI::Slider::Max>(1.0f)
			)
		.property("SunZ", &S_SunLight::sunZ)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Sun Z:"),
				RegisterMetamember<UI::Slider::Min>(-1.0f),
				RegisterMetamember<UI::Slider::Max>(1.0f)
			)
		.property("SunColor", &S_SunLight::sunColor)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Colour>(),
				RegisterMetamember<UI::Colour::Name>("Sun color"),
				REGISTER_DEFAULT_VALUE(GLEngine::Colours::white)
			)
		.property("AsymmetricFactor", &S_SunLight::asymmetricFactor)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Asymmetric factor:"),
				RegisterMetamember<UI::Slider::Min>(0.f),
				RegisterMetamember<UI::Slider::Max>(1.0f)
			)
		.property("DiscMultiplier", &S_SunLight::sunDiscMultiplier)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Disc multiplier:"),
				RegisterMetamember<UI::Slider::Min>(1.f),
				RegisterMetamember<UI::Slider::Max>(20.0f),
				REGISTER_DEFAULT_VALUE(1.f)
			);
}
// clang-format on

namespace {
const bool g_RegisterLightComponents = [] {
	using namespace GLEngine::Entity;
	C_ComponentRegistry::Instance().Register<GLEngine::Renderer::S_PointLight>("PointLight");
	C_ComponentRegistry::Instance().Register<GLEngine::Renderer::S_AreaLight>("AreaLight");
	C_ComponentRegistry::Instance().Register<GLEngine::Renderer::S_SunLight>("SunLight");
	return true;
}();
} // namespace

namespace GLEngine::Renderer {
void EnsureLightComponentsRegistered()
{
}
} // namespace GLEngine::Renderer

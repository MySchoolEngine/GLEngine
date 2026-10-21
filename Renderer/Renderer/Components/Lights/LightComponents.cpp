#include <RendererStdafx.h>

#include <Renderer/Components/Lights/LightComponents.h>

#include <Entity/ComponentRegistry.h>

#include <Utils/Serialization/SerializationUtils.h>

#include <rttr/registration>

RTTR_REGISTRATION
{
	using namespace GLEngine::Renderer;

	rttr::registration::class_<S_PointLight>("PointLight")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("Intensity", &S_PointLight::intensity)
		.property("Color", &S_PointLight::color);

	rttr::registration::class_<S_AreaLight>("AreaLight")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("Width", &S_AreaLight::width)
		.property("Height", &S_AreaLight::height)
		.property("DiffuseColour", &S_AreaLight::diffuseColor)
		.property("SpecColour", &S_AreaLight::specularColor);

	rttr::registration::class_<S_SunLight>("SunLight")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("SunX", &S_SunLight::sunX)
		.property("SunY", &S_SunLight::sunY)
		.property("SunZ", &S_SunLight::sunZ)
		.property("SunColor", &S_SunLight::sunColor)
		.property("AsymmetricFactor", &S_SunLight::asymmetricFactor)
		.property("DiscMultiplier", &S_SunLight::sunDiscMultiplier)(REGISTER_DEFAULT_VALUE(1.f));
}

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

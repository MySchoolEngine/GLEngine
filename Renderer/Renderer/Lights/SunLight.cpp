#include <RendererStdafx.h>

#include <Renderer/DebugDraw.h>
#include <Renderer/Lights/SunLight.h>

#include <GUI/ReflectionGUI.h>

#include <Physics/Primitives/Frustum.h>

#include <Utils/Reflection/Metadata.h>
#include <Utils/Serialization/SerializationUtils.h>

#pragma region registration
// clang-format off
RTTR_REGISTRATION
{
	using namespace Utils::Reflection;
	using namespace GLEngine::Renderer;

	rttr::registration::class_<C_SunLight>("C_SunLight")
		.constructor<std::shared_ptr<GLEngine::Entity::I_Entity>>()(rttr::policy::ctor::as_std_shared_ptr)
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("SunX", &C_SunLight::m_SunX)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Sun X:"),
				RegisterMetamember<UI::Slider::Min>(-1.0f),
				RegisterMetamember<UI::Slider::Max>(1.0f)
			)
		.property("SunY", &C_SunLight::m_SunY)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Sun Y:"),
				RegisterMetamember<UI::Slider::Min>(-1.0f),
				RegisterMetamember<UI::Slider::Max>(1.0f)
			)
		.property("SunZ", &C_SunLight::m_SunZ)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Sun Z:"),
				RegisterMetamember<UI::Slider::Min>(-1.0f),
				RegisterMetamember<UI::Slider::Max>(1.0f)
			)
		.property("SunColor", &C_SunLight::m_SunColor)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Colour>(),
				RegisterMetamember<UI::Colour::Name>("Sun color"),
				REGISTER_DEFAULT_VALUE(GLEngine::Colours::white)
			)
		.property("AsymmetricFactor", &C_SunLight::m_AsymetricFactor)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Asymmetric factor:"),
				RegisterMetamember<UI::Slider::Min>(0.f),
				RegisterMetamember<UI::Slider::Max>(1.0f)
			)
		.property("DiscMultiplier", &C_SunLight::m_AsymetricFactor)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Disc multiplier:"),
				RegisterMetamember<UI::Slider::Min>(1.f),
				RegisterMetamember<UI::Slider::Max>(20.0f),
				REGISTER_DEFAULT_VALUE(1.f)
			);

	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<C_SunLight>>();
	rttr::type::register_converter_func([](std::shared_ptr<C_SunLight> ptr, bool& ok) 
		-> std::shared_ptr<GLEngine::Entity::I_Component> {
		ok = true;
		return std::static_pointer_cast<GLEngine::Entity::I_Component>(ptr);
	});
}
// clang-format on
#pragma endregion registration

namespace GLEngine::Renderer {

//=================================================================================
C_SunLight::C_SunLight(const std::shared_ptr<Entity::I_Entity>& owner)
	: I_Light(owner)
	, m_SunX(0.437f)
	, m_SunY(0.056f)
	, m_SunZ(-0.347f)
	, m_SunColor(Colours::white)
	, m_AsymetricFactor(0.95f)
	, m_SunDiscMultiplier(1.f)
{
}

//=================================================================================
C_SunLight::C_SunLight()
	: C_SunLight(nullptr)
{
}

//=================================================================================
C_SunLight::~C_SunLight() = default;

//=================================================================================
bool C_SunLight::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
std::string_view C_SunLight::GetDebugComponentName() const
{
	return "Sun";
}

//=================================================================================
void C_SunLight::DebugDrawGUI()
{
	rttr::instance obj(*this);
	GUI::DrawAllPropertyGUI(obj);
}

//=================================================================================
void C_SunLight::DebugDraw(I_DebugDraw* dd) const
{
	dd->DrawPoint({m_SunX, m_SunY, m_SunZ}, Colours::yellow);
	dd->DrawLine({0.f, 0.f, 0.f}, {m_SunX, m_SunY, m_SunZ}, Colours::yellow);
}

//=================================================================================
glm::vec3 C_SunLight::GetSunDirection() const
{
	return {m_SunX, m_SunY, m_SunZ};
}

//=================================================================================
Colours::T_Colour C_SunLight::GetSunColor() const
{
	return m_SunColor;
}

//=================================================================================
float C_SunLight::SunDiscMultiplier() const
{
	return m_SunDiscMultiplier;
}

//=================================================================================
float C_SunLight::AtmosphereAsymetricFactor() const
{
	return m_AsymetricFactor;
}

//=================================================================================
Physics::Primitives::C_Frustum C_SunLight::GetShadingFrustum() const
{
	return Physics::Primitives::C_Frustum(glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f), 0, 0, 0, 0);
}

//=================================================================================
Physics::Primitives::S_AABB C_SunLight::GetAABB() const
{
	return Physics::Primitives::S_AABB();
}

} // namespace GLEngine::Renderer
